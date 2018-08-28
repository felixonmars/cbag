/** \file database.cpp
 *  \brief This file implements classes and functions to read/write an
 * OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <fstream>

#include <boost/filesystem.hpp>

#include <spdlog/spdlog.h>

#include <fmt/format.h>

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/instance.h>
#include <cbagyaml/cbagyaml.h>

#include <cbagoa/oa_database.h>
#include <cbagoa/oa_reader.h>
#include <cbagoa/oa_writer.h>

namespace fs = boost::filesystem;

namespace cbagoa {

void oa_database::handle_oa_exceptions() {
    logger->error("Exception caught, exiting");
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

oa::oaBoolean LibDefObserver::onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                             oa::oaLibDefListWarningTypeEnum type) {
    throw std::runtime_error("OA Error: " + std::string(msg));
}

oa_database::oa_database(const std::string &lib_def_file) : oa_database(lib_def_file.c_str()) {}

oa_database::oa_database(const char *lib_def_file)
    : lib_def_file(lib_def_file), lib_def_obs(1), logger(spdlog::get("cbag")) {
    try {

        reader = std::make_unique<oa_reader>(ns_cdba, logger);
        writer = std::make_unique<oa_writer>(ns_cdba, logger);

        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, oacDataModelRevNumber);

        logger->info("Creating new oa_database with file: {}", lib_def_file);
        oa::oaLibDefList::openLibs(lib_def_file);
    } catch (...) {
        handle_oa_exceptions();
    }
}

oa_database::~oa_database() {
    try {
        logger->info("Closing all OA libraries from definition file: {}", lib_def_file);
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

std::vector<std::string> oa_database::get_cells_in_library(const char *library) {
    std::vector<std::string> ans;
    try {
        oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, library);
        oa::oaLib *lib_ptr = oa::oaLib::find(lib_name_oa);
        if (lib_ptr == nullptr) {
            throw std::invalid_argument(fmt::format("Cannot find library {}", library));
        }
        if (!lib_ptr->getAccess(oa::oacReadLibAccess, LIB_ACCESS_TIMEOUT)) {
            throw std::runtime_error(
                fmt::format("Cannot obtain read access to library: {}", library));
        }
        oa::oaIter<oa::oaCell> cell_iter(lib_ptr->getCells());
        oa::oaCell *cell_ptr;
        oa::oaString tmp_str;
        while ((cell_ptr = cell_iter.getNext()) != nullptr) {
            cell_ptr->getName(ns, tmp_str);
            ans.emplace_back(tmp_str);
        }
        lib_ptr->releaseAccess();
    } catch (...) {
        handle_oa_exceptions();
    }
    return ans;
}

std::string oa_database::get_lib_path(const char *library) {
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

void oa_database::create_lib(const char *library, const char *lib_path, const char *tech_lib) {
    try {
        logger->info("Creating OA library {}", library);

        // open library
        oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, library);
        oa::oaLib *lib_ptr = oa::oaLib::find(lib_name_oa);
        if (lib_ptr == nullptr) {
            // create new library
            logger->info("Creating library {} at path {}, with tech lib {}", library, lib_path,
                         tech_lib);
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
            logger->info("Library already exists, do nothing.");
        }
    } catch (...) {
        handle_oa_exceptions();
    }
}

cbag::sch::cellview oa_database::read_sch_cellview(const char *lib_name, const char *cell_name,
                                                   const char *view_name) {
    try {
        oa::oaDesign *dsn_ptr = open_design(lib_name, cell_name, view_name, 'r');
        logger->info("Reading cellview {}__{}({})", lib_name, cell_name, view_name);
        cbag::sch::cellview ans = reader->read_sch_cellview(dsn_ptr);
        dsn_ptr->close();
        return ans;
    } catch (...) {
        handle_oa_exceptions();
        throw;
    }
}

cbag::sch::cellview oa_database::read_sch_cellview(const std::string &lib_name,
                                                   const std::string &cell_name,
                                                   const std::string &view_name) {
    return read_sch_cellview(lib_name.c_str(), cell_name.c_str(), view_name.c_str());
}

std::vector<cell_key_t>
oa_database::read_sch_recursive(const char *lib_name, const char *cell_name, const char *view_name,
                                const char *new_root_path, const str_map_t &lib_map,
                                const std::unordered_set<std::string> &exclude_libs) {
    std::pair<std::string, std::string> key(lib_name, cell_name);
    cell_set_t exclude_cells;
    std::vector<cell_key_t> ans;
    read_sch_helper(key, view_name, new_root_path, lib_map, exclude_libs, exclude_cells, ans);
    return ans;
}

std::vector<cell_key_t>
oa_database::read_library(const char *lib_name, const char *view_name, const char *new_root_path,
                          const str_map_t &lib_map,
                          const std::unordered_set<std::string> &exclude_libs) {
    cell_set_t exclude_cells;
    std::vector<cell_key_t> ans;
    for (auto const &cell_name : get_cells_in_library(lib_name)) {
        std::pair<std::string, std::string> key(lib_name, cell_name);
        read_sch_helper(key, view_name, new_root_path, lib_map, exclude_libs, exclude_cells, ans);
    }
    return ans;
}

void oa_database::write_sch_cellview(const char *lib_name, const char *cell_name,
                                     const char *view_name, bool is_sch,
                                     const cbag::sch::cellview &cv, const str_map_t *rename_map) {
    try {
        oa::oaDesign *dsn_ptr = open_design(lib_name, cell_name, view_name, 'w', is_sch);
        logger->info("Writing cellview {}__{}({})", lib_name, cell_name, view_name);
        writer->write_sch_cellview(cv, dsn_ptr, is_sch, rename_map);
        dsn_ptr->close();
    } catch (...) {
        handle_oa_exceptions();
    }
}

void oa_database::implement_sch_list(const char *lib_name,
                                     const std::vector<std::string> &cell_list,
                                     const char *sch_view, const char *sym_view,
                                     const std::vector<cbag::sch::cellview *> &cv_list) {
    try {
        str_map_t rename_map;

        std::size_t num = cell_list.size();
        for (std::size_t idx = 0; idx < num; ++idx) {
            const char *cell_name = cell_list[idx].c_str();
            write_sch_cellview(lib_name, cell_name, sch_view, true, *(cv_list[idx]), &rename_map);
            if (cv_list[idx]->sym_ptr != nullptr && sym_view != nullptr) {
                write_sch_cellview(lib_name, cell_name, sym_view, false, *(cv_list[idx]->sym_ptr));
            }
            logger->info("cell name {} maps to {}", cv_list[idx]->cell_name, cell_list[idx]);
            rename_map[cv_list[idx]->cell_name] = cell_list[idx];
        }
    } catch (...) {
        handle_oa_exceptions();
    }
}

oa::oaTech *oa_database::read_tech(const char *library) {
    // open technology file
    oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, library);
    oa::oaTech *tech_ptr = oa::oaTech::find(lib_name_oa);
    if (tech_ptr == nullptr) {
        // opened tech not found, attempt to open
        if (!oa::oaTech::exists(lib_name_oa)) {
            throw std::runtime_error(fmt::format("Cannot find technology library: {}", library));
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

oa::oaDesign *oa_database::open_design(const char *lib_name, const char *cell_name,
                                       const char *view_name, char mode, bool is_sch) {
    oa::oaScalarName lib_oa(ns, lib_name);
    oa::oaScalarName cell_oa(ns, cell_name);
    oa::oaScalarName view_oa(ns, view_name);

    logger->info("Opening design {}__{}({}) with mode {}", lib_name, cell_name, view_name, mode);
    oa::oaDesign *dsn_ptr = nullptr;
    if (mode == 'r') {
        dsn_ptr = oa::oaDesign::open(lib_oa, cell_oa, view_oa, mode);
    } else {
        oa::oaViewType *view_type =
            oa::oaViewType::get((is_sch) ? oa::oacSchematic : oa::oacSchematicSymbol);
        dsn_ptr = oa::oaDesign::open(lib_oa, cell_oa, view_oa, view_type, mode);
    }
    if (dsn_ptr == nullptr) {
        throw std::invalid_argument(fmt::format("Cannot open cell: {}__{}({}) with mode {}",
                                                lib_name, cell_name, view_name, mode));
    }
    return dsn_ptr;
}

void oa_database::read_sch_helper(std::pair<std::string, std::string> &key, const char *view_name,
                                  const char *new_root_path, const str_map_t &lib_map,
                                  const std::unordered_set<std::string> &exclude_libs,
                                  cell_set_t &exclude_cells, std::vector<cell_key_t> &cell_list) {
    // find root_path
    str_map_t::const_iterator map_iter;
    map_iter = lib_map.find(key.first);
    std::string root_path =
        (map_iter != lib_map.cend()) ? map_iter->second : std::string(new_root_path);

    // write cellviews to yaml files
    cbag::sch::cellview sch_cv =
        cell_to_yaml(key.first.c_str(), key.second.c_str(), view_name, root_path);

    // update cell_list and exclude_cells
    cell_list.push_back(key);
    exclude_cells.insert(std::move(key));

    // recurse
    auto exc_lib_end = exclude_libs.end();
    for (const auto &pair : sch_cv.instances) {
        std::pair<std::string, std::string> ikey(pair.second.lib_name, pair.second.cell_name);
        if (exclude_cells.find(ikey) == exclude_cells.end()) {
            // did not see this schematic master before
            if (exclude_libs.find(pair.second.lib_name) == exc_lib_end) {
                // non-primitive master, parse normally
                read_sch_helper(ikey, view_name, new_root_path, lib_map, exclude_libs,
                                exclude_cells, cell_list);
            }
        }
    }
}

cbag::sch::cellview oa_database::cell_to_yaml(const std::string &lib_name,
                                              const std::string &cell_name, const char *sch_view,
                                              const std::string &root_path) {
    // create directory if not exist, then compute output filename
    fs::path root_dir(fs::path(root_path) / lib_name / "netlist_info");
    fs::create_directories(root_dir);

    // parse schematic
    cbag::sch::cellview sch_cv = read_sch_cellview(lib_name.c_str(), cell_name.c_str(), sch_view);

    // write schematic to file
    fs::path tmp_path = root_dir / fmt::format("{}.yaml", cell_name);
    cbag::to_file(sch_cv, tmp_path.c_str());

    // write all symbol views to file
    // get library read access
    oa::oaLib *lib_ptr = oa::oaLib::find(oa::oaScalarName(ns_cdba, lib_name.c_str()));
    if (!lib_ptr->getAccess(oa::oacReadLibAccess, LIB_ACCESS_TIMEOUT)) {
        throw std::runtime_error(fmt::format("Cannot obtain read access to library: {}", lib_name));
    }
    // find all symbol views
    oa::oaScalarName cell_name_oa(ns_cdba, cell_name.c_str());
    oa::oaCell *cell_ptr = oa::oaCell::find(lib_ptr, cell_name_oa);
    oa::oaIter<oa::oaCellView> cv_iter(cell_ptr->getCellViews());
    oa::oaCellView *cv_ptr;
    while ((cv_ptr = cv_iter.getNext()) != nullptr) {
        oa::oaString tmp_name;
        oa::oaView *view_ptr = cv_ptr->getView();
        if (view_ptr->getViewType() == oa::oaViewType::get(oa::oacSchematicSymbol)) {
            view_ptr->getName(ns_cdba, tmp_name);
            tmp_path = root_dir / fmt::format("{}.{}.yaml", cell_name, (const char *)tmp_name);
            cbag::to_file(read_sch_cellview(lib_name.c_str(), cell_name.c_str(), tmp_name),
                          tmp_path.c_str());
        }
    }
    // release read access
    lib_ptr->releaseAccess();

    return sch_cv;
}

} // namespace cbagoa
