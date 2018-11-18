#include <catch2/catch.hpp>

#include <fmt/core.h>

#include <cbag/cbag.h>
#include <cbagyaml/cbagyaml.h>

cbag::sch::cellview read_yaml(const std::string &fname) { return cbag::cv_from_file(fname); }

SCENARIO("netlist generation", "[cbag]") {
    GIVEN("cellviews from yaml files") {
        std::string yaml_dir = "tests/data/test_netlist_yaml";
        std::string output_dir = "tests/data/test_outputs/netlist";

        std::string cell_name = GENERATE(values<std::string>({
            "cv_simple", "cv_bus_term",
            //"cv_inst_w_bus",
            //"cv_array_inst_simple",
            //"cv_array_inst_w_bus",
        }));

        auto cv = read_yaml(fmt::format("{}/{}.yaml", yaml_dir, cell_name));

        std::vector<sch::cellview *> cv_list = {&cv};
        std::vector<std::string> name_list = {"TEST"};
        std::vector<std::string> inc_list;
        cbag::netlist_map_t netlist_map;
        bool flat = false;
        bool shell = false;
        std::string fname = fmt::format("{}/{}.netlist", output_dir, cell_name);

        THEN("can write cdl netlist") {
            write_netlist(cv_list, name_list, inc_list, netlist_map, flat, shell,
                          cbag::netlist_fmt::CDL, fname);
        }
    }
}
