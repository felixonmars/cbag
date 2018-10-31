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
#include <yaml-cpp/yaml.h>

#include <cbag/layout/cellview.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/instance.h>
#include <cbagyaml/cbagyaml.h>

#include <cbagoa/oa_database.h>
#include <cbagoa/oa_read.h>
#include <cbagoa/oa_write.h>

namespace fs = boost::filesystem;

namespace cbagoa {

struct oa_database::helper {
    static void handle_oa_exceptions(const std::shared_ptr<spdlog::logger> &logger) {
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

    static oa::oaTech *read_tech(const oa::oaNativeNS &ns, const char *library) {
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

    static oa::oaDesign *open_design(const oa::oaNativeNS &ns,
                                     const std::shared_ptr<spdlog::logger> logger,
                                     const char *lib_name, const char *cell_name,
                                     const char *view_name, char mode,
                                     oa::oaReservedViewTypeEnum view_enum) {
        oa::oaScalarName lib_oa(ns, lib_name);
        oa::oaScalarName cell_oa(ns, cell_name);
        oa::oaScalarName view_oa(ns, view_name);

        logger->info("Opening design {}__{}({}) with mode {}", lib_name, cell_name, view_name,
                     mode);
        oa::oaDesign *dsn_ptr = nullptr;
        if (mode == 'r') {
            dsn_ptr = oa::oaDesign::open(lib_oa, cell_oa, view_oa, mode);
        } else {
            oa::oaViewType *view_type = oa::oaViewType::get(view_enum);
            dsn_ptr = oa::oaDesign::open(lib_oa, cell_oa, view_oa, view_type, mode);
        }
        if (dsn_ptr == nullptr) {
            throw std::invalid_argument(fmt::format("Cannot open cell: {}__{}({}) with mode {}",
                                                    lib_name, cell_name, view_name, mode));
        }
        return dsn_ptr;
    }

    static cbag::sch::cellview cell_to_yaml(const oa_database &self, const std::string &lib_name,
                                            const std::string &cell_name, const char *sch_view,
                                            const std::string &root_path) {
        // create directory if not exist, then compute output filename
        fs::path root_dir(fs::path(root_path) / lib_name / "netlist_info");
        fs::create_directories(root_dir);

        // parse schematic
        cbag::sch::cellview sch_cv =
            self.read_sch_cellview(lib_name.c_str(), cell_name.c_str(), sch_view);

        // write schematic to file
        fs::path tmp_path = root_dir / fmt::format("{}.yaml", cell_name);
        cbag::to_file(sch_cv, tmp_path.c_str());

        // write all symbol views to file
        // get library read access
        oa::oaLib *lib_ptr = oa::oaLib::find(oa::oaScalarName(self.ns_cdba, lib_name.c_str()));
        if (!lib_ptr->getAccess(oa::oacReadLibAccess, LIB_ACCESS_TIMEOUT)) {
            throw std::runtime_error(
                fmt::format("Cannot obtain read access to library: {}", lib_name));
        }
        // find all symbol views
        oa::oaScalarName cell_name_oa(self.ns_cdba, cell_name.c_str());
        oa::oaCell *cell_ptr = oa::oaCell::find(lib_ptr, cell_name_oa);
        oa::oaIter<oa::oaCellView> cv_iter(cell_ptr->getCellViews());
        oa::oaCellView *cv_ptr;
        while ((cv_ptr = cv_iter.getNext()) != nullptr) {
            oa::oaString tmp_name;
            oa::oaView *view_ptr = cv_ptr->getView();
            if (view_ptr->getViewType() == oa::oaViewType::get(oa::oacSchematicSymbol)) {
                view_ptr->getName(self.ns_cdba, tmp_name);
                tmp_path = root_dir / fmt::format("{}.{}.yaml", cell_name, (const char *)tmp_name);
                cbag::to_file(self.read_sch_cellview(lib_name.c_str(), cell_name.c_str(), tmp_name),
                              tmp_path.c_str());
            }
        }
        // release read access
        lib_ptr->releaseAccess();

        return sch_cv;
    }

    static void read_sch_helper(const oa_database &self, std::pair<std::string, std::string> &key,
                                const char *view_name, const char *new_root_path,
                                const str_map_t &lib_map,
                                const std::unordered_set<std::string> &exclude_libs,
                                cell_set_t &exclude_cells, std::vector<cell_key_t> &cell_list) {
        // find root_path
        str_map_t::const_iterator map_iter;
        map_iter = lib_map.find(key.first);
        std::string root_path =
            (map_iter != lib_map.cend()) ? map_iter->second : std::string(new_root_path);

        // write cellviews to yaml files
        cbag::sch::cellview sch_cv =
            cell_to_yaml(self, key.first.c_str(), key.second.c_str(), view_name, root_path);

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
                    read_sch_helper(self, ikey, view_name, new_root_path, lib_map, exclude_libs,
                                    exclude_cells, cell_list);
                }
            }
        }
    }
};

oa::oaBoolean LibDefObserver::onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                             oa::oaLibDefListWarningTypeEnum type) {
    throw std::runtime_error("OA Error: " + std::string(msg));
}

oa_database::oa_database(const std::string &lib_def_file) : oa_database(lib_def_file.c_str()) {}

oa_database::oa_database(const char *lib_def_file)
    : lib_def_file(lib_def_file), logger(spdlog::get("cbag")) {
    try {

        writer = std::make_unique<oa_writer>(ns_cdba, logger);

        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, oacDataModelRevNumber);

        logger->info("Creating new oa_database with file: {}", lib_def_file);
        oa::oaLibDefList::openLibs(lib_def_file);
    } catch (...) {
        helper::handle_oa_exceptions(logger);
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
        helper::handle_oa_exceptions(logger);
    }
}

std::vector<std::string> oa_database::get_cells_in_library(const char *library) const {
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
        helper::handle_oa_exceptions(logger);
    }
    return ans;
}

std::string oa_database::get_lib_path(const char *library) const {
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
        helper::handle_oa_exceptions(logger);
    }
    // should never get here
    return "";
}

void oa_database::create_lib(const char *library, const char *lib_path,
                             const char *tech_lib) const {
    try {
        logger->info("Creating OA library {}", library);

        // open library
        oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, library);
        oa::oaLib *lib_ptr = oa::oaLib::find(lib_name_oa);
        if (lib_ptr == nullptr) {
            // append library name to lib_path
            fs::path new_lib_path(lib_path);
            new_lib_path /= library;
            if (new_lib_path.has_parent_path()) {
                fs::create_directories(new_lib_path.parent_path());
            }

            // create new library
            logger->info("Creating library {} at path {}, with tech lib {}", library,
                         new_lib_path.c_str(), tech_lib);

            oa::oaScalarName oa_tech_lib(ns, tech_lib);
            lib_ptr = oa::oaLib::create(lib_name_oa, new_lib_path.c_str());
            oa::oaTech::attach(lib_ptr, oa_tech_lib);

            // NOTE: I cannot get open access to modify the library file, so
            // we just do it by hand.
            std::ofstream outfile;
            outfile.open(lib_def_file, std::ios_base::app);
            outfile << "DEFINE " << library << " " << new_lib_path << std::endl;
            outfile.close();

            // Create cdsinfo.tag file
            outfile.open((new_lib_path / "cdsinfo.tag").string(), std::ios_base::out);
            outfile << "CDSLIBRARY" << std::endl;
            outfile << "NAMESPACE LibraryUnix" << std::endl;
            outfile.close();
        } else {
            logger->info("Library already exists, do nothing.");
        }
    } catch (...) {
        helper::handle_oa_exceptions(logger);
    }
}

cbag::sch::cellview oa_database::read_sch_cellview(const char *lib_name, const char *cell_name,
                                                   const char *view_name) const {
    try {
        oa::oaDesign *dsn_ptr =
            helper::open_design(ns, logger, lib_name, cell_name, view_name, 'r', oa::oacSchematic);
        logger->info("Reading cellview {}__{}({})", lib_name, cell_name, view_name);
        cbag::sch::cellview ans = cbagoa::read_sch_cellview(ns_cdba, *logger, dsn_ptr);
        dsn_ptr->close();
        return ans;
    } catch (...) {
        helper::handle_oa_exceptions(logger);
        throw;
    }
}

std::vector<cell_key_t>
oa_database::read_sch_recursive(const char *lib_name, const char *cell_name, const char *view_name,
                                const char *new_root_path, const str_map_t &lib_map,
                                const std::unordered_set<std::string> &exclude_libs) const {
    std::pair<std::string, std::string> key(lib_name, cell_name);
    cell_set_t exclude_cells;
    std::vector<cell_key_t> ans;
    helper::read_sch_helper(*this, key, view_name, new_root_path, lib_map, exclude_libs,
                            exclude_cells, ans);
    return ans;
}

std::vector<cell_key_t>
oa_database::read_library(const char *lib_name, const char *view_name, const char *new_root_path,
                          const str_map_t &lib_map,
                          const std::unordered_set<std::string> &exclude_libs) const {
    cell_set_t exclude_cells;
    std::vector<cell_key_t> ans;
    for (auto const &cell_name : get_cells_in_library(lib_name)) {
        std::pair<std::string, std::string> key(lib_name, cell_name);
        helper::read_sch_helper(*this, key, view_name, new_root_path, lib_map, exclude_libs,
                                exclude_cells, ans);
    }
    return ans;
}

void oa_database::write_sch_cellview(const char *lib_name, const char *cell_name,
                                     const char *view_name, bool is_sch,
                                     const cbag::sch::cellview &cv,
                                     const str_map_t *rename_map) const {
    try {
        oa::oaDesign *dsn_ptr =
            helper::open_design(ns, logger, lib_name, cell_name, view_name, 'w',
                                is_sch ? oa::oacSchematic : oa::oacSchematicSymbol);
        logger->info("Writing cellview {}__{}({})", lib_name, cell_name, view_name);
        writer->write_sch_cellview(cv, dsn_ptr, is_sch, rename_map);
        dsn_ptr->close();
    } catch (...) {
        helper::handle_oa_exceptions(logger);
    }
}

void oa_database::implement_sch_list(const char *lib_name,
                                     const std::vector<std::string> &cell_list,
                                     const char *sch_view, const char *sym_view,
                                     const std::vector<cbag::sch::cellview *> &cv_list) const {
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
        helper::handle_oa_exceptions(logger);
    }
}

void oa_database::write_lay_cellview(const char *lib_name, const char *cell_name,
                                     const char *view_name, const cbag::layout::cellview &cv,
                                     const str_map_t *rename_map, oa::oaTech *tech_ptr) const {
    try {
        if (tech_ptr == nullptr) {
            tech_ptr = helper::read_tech(ns, lib_name);
        }

        oa::oaDesign *dsn_ptr =
            helper::open_design(ns, logger, lib_name, cell_name, view_name, 'w', oa::oacMaskLayout);
        logger->info("Writing cellview {}__{}({})", lib_name, cell_name, view_name);
        writer->write_lay_cellview(cv, dsn_ptr, tech_ptr, rename_map);
        dsn_ptr->close();
    } catch (...) {
        helper::handle_oa_exceptions(logger);
    }
}

void oa_database::implement_lay_list(const char *lib_name,
                                     const std::vector<std::string> &cell_list, const char *view,
                                     const std::vector<cbag::layout::cellview *> &cv_list) const {
    try {
        str_map_t rename_map;
        auto *tech_ptr = helper::read_tech(ns, lib_name);

        std::size_t num = cell_list.size();
        for (std::size_t idx = 0; idx < num; ++idx) {
            const char *cell_name = cell_list[idx].c_str();
            write_lay_cellview(lib_name, cell_name, view, *(cv_list[idx]), &rename_map, tech_ptr);
            logger->info("cell name {} maps to {}", cv_list[idx]->cell_name, cell_list[idx]);
            rename_map[cv_list[idx]->cell_name] = cell_list[idx];
        }
    } catch (...) {
        helper::handle_oa_exceptions(logger);
    }
} // namespace cbagoa

void oa_database::write_tech_info_file(const char *fname, const char *tech_lib,
                                       const char *pin_purpose) {
    oa::oaTech *tech_ptr = helper::read_tech(ns, tech_lib);

    // read layer/purpose/via mappings
    std::map<oa::oaLayerNum, std::string> lay_map;
    std::map<oa::oaPurposeNum, std::string> pur_map;
    std::map<std::pair<oa::oaLayerNum, oa::oaLayerNum>, std::string> via_map;

    oa::oaString tmp;
    oa::oaIter<oa::oaLayer> lay_iter(tech_ptr->getLayers());
    oa::oaLayer *lay;
    while ((lay = lay_iter.getNext()) != nullptr) {
        lay->getName(tmp);
        lay_map.emplace(lay->getNumber(), std::string(tmp));
    }

    oa::oaIter<oa::oaPurpose> pur_iter(tech_ptr->getPurposes());
    oa::oaPurpose *pur;
    while ((pur = pur_iter.getNext()) != nullptr) {
        pur->getName(tmp);
        pur_map.emplace(pur->getNumber(), std::string(tmp));
    }

    oa::oaIter<oa::oaViaDef> via_iter(tech_ptr->getViaDefs());
    oa::oaViaDef *via;
    while ((via = via_iter.getNext()) != nullptr) {
        via->getName(tmp);
        via_map.emplace(std::make_pair(via->getLayer1Num(), via->getLayer2Num()), std::string(tmp));
    }

    // emit to YAML
    YAML::Emitter out;
    out.SetSeqFormat(YAML::Flow);
    out << YAML::BeginMap;

    out << YAML::Key << "options" << YAML::Value;
    out << YAML::BeginMap;
    out << YAML::Key << "default_purpose" << YAML::Value << "drawing";
    out << YAML::Key << "pin_purpose" << YAML::Value << pin_purpose;
    out << YAML::Key << "make_pin_obj" << YAML::Value << true;
    out << YAML::EndMap;

    out << YAML::Key << "layer" << YAML::Value;
    out << YAML::BeginMap;
    for (auto const &p : lay_map) {
        out << YAML::Key << p.second << YAML::Value << p.first;
    }
    out << YAML::EndMap;

    out << YAML::Key << "purpose" << YAML::Value;
    out << YAML::BeginMap;
    for (auto const &p : pur_map) {
        out << YAML::Key << p.second << YAML::Value << p.first;
    }
    out << YAML::EndMap;

    out << YAML::Key << "via_layers" << YAML::Value;
    out << YAML::BeginMap;
    for (auto const &p : via_map) {
        out << YAML::Key << p.second << YAML::Value;
        out << YAML::BeginSeq << p.first.first << p.first.second << YAML::EndSeq;
    }
    out << YAML::EndMap;

    out << YAML::EndMap;

    // write to file
    fs::path file_path(fname);
    if (file_path.has_parent_path()) {
        fs::create_directories(file_path.parent_path());
    }
    std::ofstream out_file(fname, std::ios_base::out);
    out_file << out.c_str();
    out_file.close();
}

} // namespace cbagoa
