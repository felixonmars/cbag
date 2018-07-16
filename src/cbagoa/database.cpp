/** \file database.cpp
 *  \brief This file implements classes and functions to read/write an OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <easylogging++.h>

#include <fmt/format.h>

#include <cbagoa/database.h>
#include <cbagoa/read_oa.h>

INITIALIZE_EASYLOGGINGPP // NOLINT


namespace cbagoa {

    void handle_oa_exceptions() {
        try {
            throw;
        } catch (oa::oaCompatibilityError &ex) {
            std::string msg_std(ex.getMsg());
            throw std::runtime_error("OA Compatibility Error: " + msg_std);
        } catch (oa::oaDMError &ex) {
            std::string msg_std(ex.getMsg());
            throw std::runtime_error("OA DM Error: " + msg_std);
        } catch (oa::oaError &ex) {
            std::string msg_std(ex.getMsg());
            throw std::runtime_error("OA Error: " + msg_std);
        } catch (oa::oaDesignError &ex) {
            throw std::runtime_error("OA Design Error: " + std::string(ex.getMsg()));
        }
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

    oa::oaBoolean LibDefObserver::onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                                 oa::oaLibDefListWarningTypeEnum type) {
        throw std::runtime_error("OA Error: " + std::string(msg));
    }

#pragma clang diagnostic pop

    OADatabase::OADatabase(const std::string &lib_def_file)
            : lib_def_file(lib_def_file), lib_def_obs(1) {
        try {

            el::Configurations logging_conf;
            logging_conf.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
            logging_conf.setGlobally(el::ConfigurationType::Filename, "cbagoa.log");
            logging_conf.setGlobally(el::ConfigurationType::MaxLogFileSize, "16777216");
            el::Loggers::reconfigureLogger("default", logging_conf);

            reader = std::make_unique<OAReader>(ns_cdba);
            writer = std::make_unique<OAWriter>(ns_cdba);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "NotAssignable"
            oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, // NOLINT
                         oacDataModelRevNumber); // NOLINT
#pragma clang diagnostic pop

            LOG(INFO) << "Creating new OADatabase with file: " << lib_def_file;
            oa::oaLibDefList::openLibs(lib_def_file.c_str());
        } catch (...) {
            handle_oa_exceptions();
        }
    }


    void OADatabase::create_lib(const std::string &library, const std::string &lib_path,
                                const std::string &tech_lib) {
        try {
            LOG(INFO) << "Creating OA library " << library;

            // open library
            oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, library.c_str());
            oa::oaLib *lib_ptr = oa::oaLib::find(lib_name_oa);
            if (lib_ptr == nullptr) {
                // create new library
                LOG(INFO) << "Creating library " << library << " at path " << lib_path
                          << ", with tech lib" << tech_lib;
                oa::oaScalarName oa_tech_lib(ns, tech_lib.c_str());
                lib_ptr = oa::oaLib::create(lib_name_oa, lib_path.c_str());
                oa::oaTech::attach(lib_ptr, oa_tech_lib);

                // NOTE: I cannot get open access to modify the library file, so
                // we just do it by hand.
                std::ofstream outfile;
                outfile.open(lib_def_file, std::ios_base::app);
                outfile << "DEFINE " << library << " " << lib_path << std::endl;
                outfile.close();
            } else {
                LOG(INFO) << "Library already exists, do nothing.";
            }
        } catch (...) {
            handle_oa_exceptions();
        }
    }

    oa::oaTech *OADatabase::read_tech(const std::string &library) {
        // open technology file
        oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, library.c_str());
        oa::oaTech *tech_ptr = oa::oaTech::find(lib_name_oa);
        if (tech_ptr == nullptr) {
            // opened tech not found, attempt to open
            if (!oa::oaTech::exists(lib_name_oa)) {
                throw std::runtime_error("Cannot find technology library: " + library);
            } else {
                tech_ptr = oa::oaTech::open(lib_name_oa, 'r');
                if (tech_ptr == nullptr) {
                    throw std::runtime_error("Cannot open technology library: " + library);
                }
            }
        }
        return tech_ptr;
    }

    OADatabase::~OADatabase() {
        LOG(INFO) << "Destroying OADatabase with file: " << lib_def_file;
    }

    oa::oaDesign *OADatabase::open_design(const std::string &lib_name, const std::string &cell_name,
                                          const std::string &view_name, char mode, bool is_sch) {
        oa::oaScalarName lib_oa(ns, lib_name.c_str());
        oa::oaScalarName cell_oa(ns, cell_name.c_str());
        oa::oaScalarName view_oa(ns, view_name.c_str());

        LOG(INFO) << fmt::format("Opening design {}__{}({}) with mode {}", lib_name,
                                 cell_name, view_name, mode);
        oa::oaDesign *dsn_ptr = nullptr;
        if (mode == 'r') {
            dsn_ptr = oa::oaDesign::open(lib_oa, cell_oa, view_oa, mode);
        } else {
            oa::oaViewType *view_type = oa::oaViewType::get(
                    (is_sch) ? oa::oacSchematic : oa::oacSchematicSymbol);
            dsn_ptr = oa::oaDesign::open(lib_oa, cell_oa, view_oa, view_type, mode);
        }
        if (dsn_ptr == nullptr) {
            throw std::invalid_argument(fmt::format("Cannot open cell: {}__{}({}) with mode {}",
                                                    lib_name, cell_name, view_name, mode));
        }
        return dsn_ptr;
    }

    cbag::SchCellView OADatabase::read_sch_cellview(const std::string &lib_name,
                                                    const std::string &cell_name,
                                                    const std::string &view_name) {
        try {
            oa::oaDesign *dsn_ptr = open_design(lib_name, cell_name, view_name, 'r');
            LOG(INFO) << fmt::format("Reading cellview {}__{}({})", lib_name, cell_name, view_name);
            cbag::SchCellView ans = reader->read_sch_cellview(dsn_ptr);
            dsn_ptr->close();
            return ans;
        } catch (...) {
            handle_oa_exceptions();
        }
    }

    void OADatabase::write_sch_cellview(const std::string &lib_name, const std::string &cell_name,
                                        const std::string &view_name, bool is_sch,
                                        const cbag::SchCellView &cv) {
        try {
            oa::oaDesign *dsn_ptr = open_design(lib_name, cell_name, view_name, 'w', is_sch);
            LOG(INFO) << fmt::format("Writing cellview {}__{}({})", lib_name, cell_name, view_name);
            writer->write_sch_cellview(cv, dsn_ptr);
            dsn_ptr->save();
            dsn_ptr->close();
        } catch (...) {
            handle_oa_exceptions();
        }
    }
}