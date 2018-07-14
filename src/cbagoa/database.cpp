/** \file database.cpp
 *  \brief This file implements classes and functions to read/write an OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

// TODO: Debug only, remve later
#include <iostream>

#include <spdlog/fmt/fmt.h>

#include <spdlog/spdlog.h>

#include <cbagoa/database.h>
#include <cbagoa/read_oa.h>


namespace cbagoa {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

    oa::oaBoolean LibDefObserver::onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                                 oa::oaLibDefListWarningTypeEnum type) {
        throw std::runtime_error("OA Error: " + std::string(msg));
    }

#pragma clang diagnostic pop

    OADatabase::OADatabase(const std::string &lib_def_file)
            : lib_def_file(lib_def_file), lib_def_obs(1) {

        spdlog::set_async_mode(8192);
        logger = spdlog::rotating_logger_st("cbagoa_logger", "cbagoa.log", 5242880, 5);
        reader = std::make_unique<OAReader>(ns_cdba, logger);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "NotAssignable"
        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, // NOLINT
                     oacDataModelRevNumber); // NOLINT
#pragma clang diagnostic pop

        logger->info("Opening all libraries in definition file: {}", lib_def_file);
        oa::oaLibDefList::openLibs(oa::oaString(lib_def_file.c_str()));
    }


    void OADatabase::create_lib(const std::string &library, const std::string &lib_path,
                                const std::string &tech_lib) {

        logger->info("Creating OA library {}", library);

        // open library
        oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, library.c_str());
        oa::oaLib *lib_ptr = oa::oaLib::find(lib_name_oa);
        if (lib_ptr == nullptr) {
            // create new library
            logger->info("Creating Library {} at path {}, with tech lib {}",
                         library, lib_path, tech_lib);
            oa::oaScalarName oa_tech_lib(ns, tech_lib.c_str());
            lib_ptr = oa::oaLib::create(lib_name_oa, oa::oaString(lib_path.c_str()));
            oa::oaTech::attach(lib_ptr, oa_tech_lib);

            // NOTE: I cannot get open access to modify the library file, so
            // we just do it by hand.
            std::ofstream outfile;
            outfile.open(lib_def_file, std::ios_base::app);
            outfile << "DEFINE " << library << " " << lib_path << std::endl;
            outfile.close();
        } else {
            logger->info("OA library {} already exists.  Do nothing.", library);
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

    oa::oaDesign *OADatabase::read_design(const std::string &lib_name, const std::string &cell_name,
                                          const std::string &view_name) {
        oa::oaScalarName lib_oa(ns, lib_name.c_str());
        oa::oaScalarName cell_oa(ns, cell_name.c_str());
        oa::oaScalarName view_oa(ns, view_name.c_str());

        logger->info("Opening design {}__{}({})", lib_name, cell_name, view_name);
        oa::oaDesign *dsn_ptr = oa::oaDesign::open(lib_oa, cell_oa, view_oa, 'r');
        if (dsn_ptr == nullptr) {
            throw std::invalid_argument(fmt::format("Cannot open cell: {}__{}({})",
                                                    lib_name, cell_name, view_name));
        }
        return dsn_ptr;
    }

    cbag::SchCellView OADatabase::read_sch_cellview(const std::string &lib_name,
                                                    const std::string &cell_name,
                                                    const std::string &view_name) {
        oa::oaDesign *dsn_ptr = read_design(lib_name, cell_name, view_name);
        logger->info("Reading cellview {}__{}({})", lib_name, cell_name, view_name);
        return reader->read_sch_cellview(dsn_ptr);
    }
}