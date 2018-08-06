/** \file cbag.cpp
 *  \brief This implements the top level header file for cbag library.
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#include <cstring>
#include <fstream>

#include <easylogging++.h>
#include <yaml-cpp/yaml.h>

#include <cbag/database/yaml_cellviews.h>
#include <cbag/netlist.h>

INITIALIZE_EASYLOGGINGPP

namespace cbag {
void init_logging() {}

void to_file(const SchCellView &cv, const char *fname) {
    std::ofstream outfile(fname, std::ios_base::out);
    YAML::Node node(cv);
    YAML::Emitter emitter;
    emitter << node;
    outfile << emitter.c_str() << std::endl;
    outfile.close();
}

void write_netlist(const std::vector<SchCellView> &cv_list,
                   const std::vector<std::string> &name_list,
                   const char *cell_map, const char *format, bool flat,
                   const char *fname) {
    YAML::Node n = YAML::LoadFile(std::string(cell_map));
    netlist_map_t netlist_map = n.as<netlist_map_t>();

    auto builder_ptr = make_netlist_builder(fname, std::string(format));
    builder_ptr->init();

    size_t num = cv_list.size();
    for (size_t idx = 0; idx < num; ++idx) {
        // add this cellview to netlist
        builder_ptr->add_cellview(name_list[idx], cv_list[idx], netlist_map);

        // add this cellview to netlist map
        auto lib_map_iter = netlist_map.find(cv_list[idx].lib_name);
        if (lib_map_iter == netlist_map.end()) {
            lib_map_iter->second.emplace(cv_list[idx].cell_name, cv_list[idx].get_info(name_list[idx]));
        } else {
            lib_map_t new_lib_map;
            new_lib_map.emplace(cv_list[idx].cell_name, cv_list[idx].get_info(name_list[idx]));
            netlist_map.emplace(cv_list[idx].lib_name, new_lib_map);
        }
    }

    // build final netlist
    builder_ptr->build();
}

} // namespace cbag
