/** \file database.cpp
 *  \brief This file implements classes and functions to read/write an
 * OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <fstream>

#include <boost/filesystem.hpp>

#include <cbag/logging/logging.h>

#include <yaml-cpp/yaml.h>

#include <cbag/util/sorted_map.h>

#include <cbag/layout/cellview.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/instance.h>
#include <cbag/util/io.h>

#include <cbag/oa/oa_database.h>
#include <cbag/oa/oa_read.h>
#include <cbag/oa/oa_read_lib.h>
#include <cbag/oa/oa_util.h>
#include <cbag/oa/oa_write.h>
#include <cbag/oa/oa_write_lib.h>

namespace fs = boost::filesystem;

namespace cbagoa {

oa::oaBoolean LibDefObserver::onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                             oa::oaLibDefListWarningTypeEnum type) {
    throw std::runtime_error("OA Error: " + std::string(msg));
}

oa_database::oa_database(std::string lib_def_fname)
    : lib_def_file(std::move(lib_def_fname)), logger(cbag::get_cbag_logger()) {
    try {
        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, oacDataModelRevNumber);

        logger->info("Creating new oa_database with file: {}", lib_def_file);
        oa::oaLibDefList::openLibs(lib_def_file.c_str());
    } catch (...) {
        handle_oa_exceptions(*logger);
    }
}

oa_database::~oa_database() {
    try {
        logger->info("Closing all OA libraries from definition file: {}", lib_def_file);
        oa::oaIter<oa::oaLib> lib_iter(oa::oaLib::getOpenLibs());
        oa::oaLib *lib_ptr;
        while ((lib_ptr = lib_iter.getNext()) != nullptr) {
            oa::oaString tmp_str;
            lib_ptr->getName(ns, tmp_str);
            lib_ptr->close();
        }
    } catch (...) {
        handle_oa_exceptions(*logger);
    }
}

void oa_database::add_yaml_path(const std::string &lib_name, std::string yaml_path) {
    yaml_path_map.insert_or_assign(lib_name, std::move(yaml_path));
}

void oa_database::add_primitive_lib(std::string lib_name) {
    primitive_libs.insert(std::move(lib_name));
}

bool oa_database::is_primitive_lib(const std::string &lib_name) const {
    return primitive_libs.find(lib_name) != primitive_libs.end();
}

std::vector<std::string> oa_database::get_cells_in_lib(const std::string &lib_name) const {
    std::vector<std::string> ans;
    cbagoa::get_cells(ns_native, *logger, lib_name, std::back_inserter(ans));
    return ans;
}

std::string oa_database::get_lib_path(const std::string &lib_name) const {
    std::string ans;
    try {
        oa::oaScalarName lib_name_oa = oa::oaScalarName(ns_native, lib_name.c_str());
        oa::oaLib *lib_ptr = oa::oaLib::find(lib_name_oa);
        if (lib_ptr == nullptr) {
            throw std::invalid_argument(fmt::format("Cannot find library {}", lib_name));
        }
        oa::oaString tmp_str;
        lib_ptr->getFullPath(tmp_str);
        return {tmp_str};
    } catch (...) {
        handle_oa_exceptions(*logger);
    }
    // should never get here
    return "";
}

void oa_database::create_lib(const std::string &lib_name, const std::string &lib_path,
                             const std::string &tech_lib) const {
    try {
        logger->info("Creating OA library {}", lib_name);

        // open library
        oa::oaScalarName lib_name_oa = oa::oaScalarName(ns_native, lib_name.c_str());
        oa::oaLib *lib_ptr = oa::oaLib::find(lib_name_oa);
        if (lib_ptr == nullptr) {
            // append library name to lib_path
            fs::path new_lib_path(lib_path);
            new_lib_path /= lib_name;
            cbag::util::make_parent_dirs(new_lib_path.string());

            // create new library
            logger->info("Creating library {} at path {}, with tech lib {}", lib_name,
                         new_lib_path.c_str(), tech_lib);

            oa::oaScalarName oa_tech_lib(ns_native, tech_lib.c_str());
            lib_ptr = oa::oaLib::create(lib_name_oa, new_lib_path.c_str());
            oa::oaTech::attach(lib_ptr, oa_tech_lib);

            // NOTE: I cannot get open access to modify the library file, so
            // we just do it by hand.
            std::ofstream outfile;
            outfile.open(lib_def_file, std::ios_base::app);
            outfile << "DEFINE " << lib_name << " " << new_lib_path << std::endl;
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
        handle_oa_exceptions(*logger);
    }
}

cbag::sch::cellview oa_database::read_sch_cellview(const std::string &lib_name,
                                                   const std::string &cell_name,
                                                   const std::string &view_name) const {
    try {
        return cbagoa::read_sch_cellview(ns_native, ns, *logger, lib_name, cell_name, view_name,
                                         primitive_libs);
    } catch (...) {
        handle_oa_exceptions(*logger);
        throw;
    }
}

std::vector<cell_key_t> oa_database::read_sch_recursive(const std::string &lib_name,
                                                        const std::string &cell_name,
                                                        const std::string &view_name) const {
    std::vector<cell_key_t> ans;
    cbagoa::read_sch_recursive(ns_native, ns, *logger, lib_name, cell_name, view_name,
                               yaml_path_map, primitive_libs, std::back_inserter(ans));
    return ans;
}

std::vector<cell_key_t> oa_database::read_library(const std::string &lib_name,
                                                  const std::string &view_name) const {
    std::vector<cell_key_t> ans;
    cbagoa::read_library(ns_native, ns, *logger, lib_name, view_name, yaml_path_map, primitive_libs,
                         std::back_inserter(ans));
    return ans;
}

void oa_database::write_sch_cellview(const std::string &lib_name, const std::string &cell_name,
                                     const std::string &view_name, bool is_sch,
                                     const cbag::sch::cellview &cv,
                                     const str_map_t *rename_map) const {
    try {
        cbagoa::write_sch_cellview(ns_native, ns, *logger, lib_name, cell_name, view_name, is_sch,
                                   cv, rename_map);
    } catch (...) {
        handle_oa_exceptions(*logger);
    }
}

void oa_database::write_lay_cellview(const std::string &lib_name, const std::string &cell_name,
                                     const std::string &view_name, const cbag::layout::cellview &cv,
                                     oa::oaTech *tech, const str_map_t *rename_map) const {
    try {
        cbagoa::write_lay_cellview(ns_native, ns, *logger, lib_name, cell_name, view_name, cv, tech,
                                   rename_map);
    } catch (...) {
        handle_oa_exceptions(*logger);
    }
}

void oa_database::implement_sch_list(const std::string &lib_name, const std::string &sch_view,
                                     const std::string &sym_view,
                                     const std::vector<sch_cv_info> &cv_list) const {
    cbagoa::implement_sch_list<std::vector<sch_cv_info>>(ns_native, ns, *logger, lib_name, sch_view,
                                                         sym_view, cv_list);
}

void oa_database::implement_lay_list(const std::string &lib_name, const std::string &view,
                                     const std::vector<lay_cv_info> &cv_list) const {
    cbagoa::implement_lay_list<std::vector<lay_cv_info>>(ns_native, ns, *logger, lib_name, view,
                                                         cv_list);
} // namespace cbagoa

void oa_database::write_tech_info_file(const std::string &fname, const std::string &tech_lib,
                                       const std::string &pin_purpose) const {
    oa::oaTech *tech_ptr = read_tech(ns_native, tech_lib);

    // read layer/purpose/via mappings
    cbag::util::sorted_map<oa::oaLayerNum, std::string> lay_map;
    cbag::util::sorted_map<oa::oaPurposeNum, std::string> pur_map;
    cbag::util::sorted_map<std::pair<oa::oaLayerNum, oa::oaLayerNum>, std::string> via_map;

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
    std::ofstream out_file = cbag::util::open_file_write(fname);
    out_file << out.c_str();
    out_file.close();
}

} // namespace cbagoa
