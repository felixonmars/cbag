#include <memory>

#include <catch2/catch.hpp>

#include <fmt/core.h>

#include <cbag/netlist/netlist.h>
#include <cbagyaml/cbagyaml.h>

void write_netlist(const std::string &yaml_dir, const std::string &cell_name) {

    const char *fmt_str = "{}/{}.yaml";

    // get dependencies
    std::vector<std::unique_ptr<cbag::sch::cellview>> cv_masters;
    std::vector<std::string> name_list;
}

SCENARIO("netlist generation", "[cbag]") {
    GIVEN("cellviews from yaml files") {
        std::string yaml_dir = "tests/data/test_netlist_yaml";
        std::string output_dir = "tests/data/test_outputs/netlist";

        std::string cell_name = GENERATE(values<std::string>({
            "nmos4_standard", "pmos4_standard",
            // "cv_simple", "cv_bus_term",
            //"cv_inst_w_bus",
            //"cv_array_inst_simple",
            //"cv_array_inst_w_bus",
        }));

        auto cv = cbag::cv_from_file(fmt::format("{}/{}.yaml", yaml_dir, cell_name));

        std::vector<std::pair<cbag::sch::cellview *, std::string>> cv_name_list = {{&cv, "TEST"}};
        std::vector<std::string> inc_list;
        cbag::netlist_map_t netlist_map;
        bool flat = false;
        bool shell = false;
        std::string fname = fmt::format("{}/{}.netlist", output_dir, cell_name);

        THEN("can write cdl netlist") {
            write_netlist(cv_name_list, inc_list, netlist_map, flat, shell, cbag::netlist_fmt::CDL,
                          fname);
        }
    }
}
