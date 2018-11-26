#include <memory>
#include <unordered_set>

#include <catch2/catch.hpp>

#include <fmt/core.h>

#include <cbag/netlist/netlist.h>

#include "util/io.h"

using name_cv_vec = std::vector<
    std::pair<std::string, std::pair<std::unique_ptr<cbag::sch::cellview>, std::string>>>;

std::string get_extension(cbag::design_output netlist_type) {
    switch (netlist_type) {
    case cbag::design_output::CDL:
        return "cdl";
    case cbag::design_output::VERILOG:
        return "v";
    default:
        return "netlist";
    }
}

void populate_name_cv_list(const char *fmt_str, const std::string &yaml_dir,
                           const std::string &top_cell, const std::string cv_name,
                           name_cv_vec &name_cv_list, std::unordered_set<std::string> &recorded) {
    auto cv = std::make_unique<cbag::sch::cellview>(fmt::format(fmt_str, yaml_dir, top_cell));

    // write subcells
    for (const auto &p : cv->instances) {
        std::string &cur_cell = p.second->cell_name;
        if (recorded.find(cur_cell) == recorded.end()) {
            populate_name_cv_list(fmt_str, yaml_dir, cur_cell, cur_cell, name_cv_list, recorded);
        }
    }

    // write this cellview
    name_cv_list.emplace_back(
        cv_name, std::pair<std::unique_ptr<cbag::sch::cellview>, std::string>(std::move(cv), ""));
    recorded.emplace(top_cell);
}

SCENARIO("netlist generation", "[cbag]") {
    GIVEN("cellviews from yaml files") {
        std::string yaml_dir = "tests/data/test_netlist_yaml";
        std::string output_dir = "tests/data/test_outputs/netlist";
        std::string expect_dir = "tests/data/test_netlist_netlist";

        std::string cell_name = GENERATE(values<std::string>({
            "nmos4_standard",
            "pmos4_standard",
            "cv_simple",
            "cv_bus_term",
            "cv_inst_w_bus",
            "cv_array_inst_simple",
            "cv_array_inst_w_bus",
        }));
        std::pair<cbag::design_output, bool> fmt_shell =
            GENERATE(values<std::pair<cbag::design_output, bool>>({
                {cbag::design_output::CDL, false},
                {cbag::design_output::VERILOG, true},
                {cbag::design_output::VERILOG, false},
            }));

        auto format = fmt_shell.first;
        bool shell = fmt_shell.second;
        const char *fmt_str = "{}/{}.yaml";
        name_cv_vec name_cv_list;
        std::unordered_set<std::string> recorded;
        populate_name_cv_list(fmt_str, yaml_dir, cell_name, "TEST", name_cv_list, recorded);
        bool flat = false;
        uint32_t rmin = 2000;

        std::string ext_str = get_extension(format);
        std::string fname = fmt::format("{}/{}_{:d}.{}", output_dir, cell_name, shell, ext_str);
        std::string expect_fname =
            fmt::format("{}/{}_{:d}.{}", expect_dir, cell_name, shell, ext_str);

        CAPTURE(cell_name);
        CAPTURE(ext_str);
        THEN("writes netlist correctly") {
            cbag::netlist::write_netlist(name_cv_list, fname, format, flat, shell, rmin);
            std::string output_str = read_file(fname);
            std::string expect_str = read_file(expect_fname);
            REQUIRE(output_str == expect_str);
        }
    }
}
