/** \file cbag.cpp
 *  \brief This implements the top level header file for cbag library.
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#include <cstring>
#include <fstream>
#include <memory>

#include <boost/filesystem.hpp>

#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include <cbagyaml/cbagyaml.h>

namespace fs = boost::filesystem;

namespace cbag {

void to_file(const sch::cellview &cv, const char *fname) {
    std::ofstream outfile(fname, std::ios_base::out);
    YAML::Node node(cv);
    YAML::Emitter emitter;
    emitter << node;
    outfile << emitter.c_str() << std::endl;
    outfile.close();
}

void from_file(const char *yaml_fname, const char *sym_view, sch::cellview &cv) {
    fs::path yaml_path(yaml_fname);
    YAML::Node n = YAML::LoadFile(yaml_path.string());
    YAML::convert<sch::cellview>::decode(n, cv);
    if (sym_view != nullptr) {
        // load symbol cellview
        yaml_path.replace_extension(fmt::format(".{}{}", sym_view, yaml_path.extension().c_str()));
        if (fs::exists(yaml_path)) {
            YAML::Node s = YAML::LoadFile(yaml_path.string());
            cv.sym_ptr = std::make_unique<sch::cellview>(s.as<sch::cellview>());
        }
    }
}

netlist_map_t read_netlist_map(const char *fname) {
    fs::path yaml_path(fname);
    YAML::Node n = YAML::LoadFile(yaml_path.string());
    return n.as<netlist_map_t>();
}

} // namespace cbag