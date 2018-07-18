/** \file database.cpp
 *  \brief This file implements classes and functions to read/write an OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <fstream>
#include <unordered_set>
#include <unordered_map>

#include <boost/filesystem.hpp>

#include <easylogging++.h>

#include <fmt/format.h>

#include <cbag/database/yaml_cellviews.h>
#include <cbagoa/read_oa.h>
#include <cbagoa/write_oa.h>
#include <cbagoa/database.h>

INITIALIZE_EASYLOGGINGPP // NOLINT


namespace fs = boost::filesystem;

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

    OADatabase::OADatabase(const char *lib_def_file)
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
            oa::oaLibDefList::openLibs(lib_def_file);
        } catch (...) {
            handle_oa_exceptions();
        }
    }

    OADatabase::~OADatabase() {
        try {
            LOG(INFO) << "Closing all OA libraries from definition file: " << lib_def_file;
            oa::oaIter<oa::oaLib> lib_iter(oa::oaLib::getOpenLibs());
            oa::oaLib *lib_ptr;
            while ((lib_ptr = lib_iter.getNext()) != nullptr) {
                oa::oaString tmp_str;
                lib_ptr->getName(ns_cdba, tmp_str);
                lib_ptr->close();
            }
        } catch (...) {
            handle_oa_exceptions();
        }
    }

    std::vector<std::string> OADatabase::get_cells_in_library(const char *library) {
        std::vector<std::string> ans;
        try {
            oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, library);
            oa::oaLib *lib_ptr = oa::oaLib::find(lib_name_oa);
            if (lib_ptr == nullptr) {
                throw std::invalid_argument(fmt::format("Cannot find library {}", library));
            }
            oa::oaIter<oa::oaCell> cell_iter(lib_ptr->getCells());
            oa::oaCell *cell_ptr;
            oa::oaString tmp_str;
            while ((cell_ptr = cell_iter.getNext()) != nullptr) {
                cell_ptr->getName(ns, tmp_str);
                ans.emplace_back(tmp_str);
            }
        } catch (...) {
            handle_oa_exceptions();
        }
        return ans;
    }

    std::string OADatabase::get_lib_path(const char *library) {
        std::string ans;
        try {
            oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, library);
            oa::oaLib *lib_ptr = oa::oaLib::find(lib_name_oa);
            if (lib_ptr == nullptr) {
                throw std::invalid_argument(fmt::format("Cannot find library {}", library));
            }
            oa::oaString tmp_str;
            lib_ptr->getFullPath(tmp_str);
            return {tmp_str};
        } catch (...) {
            handle_oa_exceptions();
        }
        // should never get here
        return "";
    }

    void OADatabase::create_lib(const char *library, const char *lib_path,
                                const char *tech_lib) {
        try {
            LOG(INFO) << "Creating OA library " << library;

            // open library
            oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, library);
            oa::oaLib *lib_ptr = oa::oaLib::find(lib_name_oa);
            if (lib_ptr == nullptr) {
                // create new library
                LOG(INFO) << "Creating library " << library << " at path " << lib_path
                          << ", with tech lib" << tech_lib;
                oa::oaScalarName oa_tech_lib(ns, tech_lib);
                lib_ptr = oa::oaLib::create(lib_name_oa, lib_path);
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

    cbag::SchCellView OADatabase::read_sch_cellview(const char *lib_name, const char *cell_name,
                                                    const char *view_name) {
        try {
            oa::oaDesign *dsn_ptr = open_design(lib_name, cell_name, view_name, 'r');
            LOG(INFO) << fmt::format("Reading cellview {}__{}({})", lib_name, cell_name, view_name);
            cbag::SchCellView ans = reader->read_sch_cellview(dsn_ptr);
            dsn_ptr->close();
            return ans;
        } catch (...) {
            handle_oa_exceptions();
            throw;
        }
    }

    std::vector<cell_key_t>
    OADatabase::read_sch_recursive(const char *lib_name, const char *cell_name,
                                   const char *view_name, const char *new_root_path,
                                   const std::unordered_map<std::string, std::string> &lib_map,
                                   const std::unordered_set<std::string> &exclude_libs) {
        std::pair<std::string, std::string> key(lib_name, cell_name);
        cell_set_t exclude_cells;
        std::vector<cell_key_t> ans;
        read_sch_helper(key, view_name, new_root_path, lib_map, exclude_libs, exclude_cells, ans);
        return ans;
    }

    void OADatabase::write_sch_cellview(const char *lib_name, const char *cell_name,
                                        const char *view_name, bool is_sch,
                                        const cbag::SchCellView &cv) {
        try {
            oa::oaDesign *dsn_ptr = open_design(lib_name, cell_name, view_name, 'w', is_sch);
            LOG(INFO) << fmt::format("Writing cellview {}__{}({})", lib_name, cell_name, view_name);
            writer->write_sch_cellview(cv, dsn_ptr);
            dsn_ptr->close();
        } catch (...) {
            handle_oa_exceptions();
        }
    }


    oa::oaTech *OADatabase::read_tech(const char *library) {
        // open technology file
        oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, library);
        oa::oaTech *tech_ptr = oa::oaTech::find(lib_name_oa);
        if (tech_ptr == nullptr) {
            // opened tech not found, attempt to open
            if (!oa::oaTech::exists(lib_name_oa)) {
                throw std::runtime_error(
                        fmt::format("Cannot find technology library: {}", library));
            } else {
                tech_ptr = oa::oaTech::open(lib_name_oa, 'r');
                if (tech_ptr == nullptr) {
                    throw std::runtime_error(
                            fmt::format("Cannot open technology library: {}", library));
                }
            }
        }
        return tech_ptr;
    }

    oa::oaDesign *OADatabase::open_design(const char *lib_name, const char *cell_name,
                                          const char *view_name, char mode, bool is_sch) {
        oa::oaScalarName lib_oa(ns, lib_name);
        oa::oaScalarName cell_oa(ns, cell_name);
        oa::oaScalarName view_oa(ns, view_name);

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


    void OADatabase::read_sch_helper(std::pair<std::string, std::string> &key,
                                     const char *view_name, const char *new_root_path,
                                     const std::unordered_map<std::string, std::string> &lib_map,
                                     const std::unordered_set<std::string> &exclude_libs,
                                     cell_set_t &exclude_cells,
                                     std::vector<cell_key_t> &cell_list) {
        // parse schematic
        cbag::SchCellView ans = read_sch_cellview(key.first.c_str(), key.second.c_str(), view_name);

        // find root_path
        auto const map_iter = lib_map.find(key.first);
        fs::path root_path(new_root_path);
        if (map_iter != lib_map.cend()) {
            root_path = fs::path(map_iter->second);
        }

        // create directory if not exist, then compute output filename
        fs::path cur_path = root_path / fs::path(key.first) / fs::path("netlist_info");
        fs::create_directories(cur_path);
        cur_path /= fs::path(key.second + ".yaml");

        // update cell_list and exclude_cells
        cell_list.push_back(key);
        exclude_cells.insert(std::move(key));

        // write to file
        std::ofstream outfile(cur_path.c_str(), std::ios_base::out);
        YAML::Node node(ans);
        YAML::Emitter emitter;
        emitter << node;
        outfile << emitter.c_str() << std::endl;
        outfile.close();

        // recurse
        auto exc_lib_end = exclude_libs.end();
        for (const auto &pair : ans.instances) {
            if (exclude_libs.find(pair.second.lib_name) == exc_lib_end) {
                std::pair<std::string, std::string> ikey(pair.second.lib_name,
                                                         pair.second.cell_name);
                if (exclude_cells.find(ikey) == exclude_cells.end()) {
                    read_sch_helper(ikey, view_name, new_root_path, lib_map, exclude_libs,
                                    exclude_cells, cell_list);
                }
            }
        }
    }


}
