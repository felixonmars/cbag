#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <catch2/catch.hpp>

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_inst_mod.h>
#include <cbag/schematic/instance.h>

using cellview = cbag::sch::cellview;
using instance = cbag::sch::instance;
using transformation = cbag::transformation;
using conn_list = std::vector<std::pair<std::string, std::string>>;
using name_conn_list = std::vector<std::pair<std::string, conn_list>>;

SCENARIO("cellview with one instance", "[cellview]") {
    cellview cv("foo", "bar", "schematic", 0, 0, 10, 10);

    cv.instances.emplace(
        "X0", std::make_unique<instance>("foo", "baz", "symbol", transformation(), 0, 0, 2, 2));

    THEN("array_instance works") {
        name_conn_list nc_list = {
            {"ZZ", conn_list()},
            {"AA", conn_list()},
            {"BB", conn_list()},
        };

        cbag::sch::array_instance(cv.instances, "X0", 0, 0, nc_list);
    }
}
