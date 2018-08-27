#include <cbag/yaml/cellviews.h>
#include <iostream>
#include <yaml-cpp/yaml.h>

int main(int argc, char *argv[]) {
    cbag::sch::cellview_info info;
    cbag::sch::lib_map_t lib_map;
    cbag::sch::netlist_map_t net_map;

    info.cell_name = "nlvt";
    info.io_terms.push_back("D");
    info.io_terms.push_back("G");
    info.io_terms.push_back("B");
    info.io_terms.push_back("S");
    info.props.emplace("nfin", std::string("4"));
    info.props.emplace("l", std::string("16n"));
    info.props.emplace("nf", std::string("2"));

    lib_map.emplace("nlvt", info);
    net_map.emplace("cds_ff_mpt", lib_map);

    YAML::Emitter emitter;
    emitter << YAML::Node(net_map);

    std::cout << emitter.c_str() << std::endl;
}
