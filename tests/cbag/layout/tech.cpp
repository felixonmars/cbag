#include <catch2/catch.hpp>

#include <cbag/layout/tech_util.h>

using c_tech = cbag::layout::tech;

SCENARIO("technology layer/purpose lookup", "[cbag]") {
    GIVEN("technology object") {
        c_tech obj("tests/data/test_layout/tech_params.yaml");

        std::pair<std::string, cbag::lay_t> lay_pair =
            GENERATE(values<std::pair<std::string, cbag::lay_t>>({
                {"M1", 60},
                {"M2", 80},
                {"M3", 100},
                {"M4", 110},
            }));

        std::pair<std::string, cbag::purp_t> purp_pair =
            GENERATE(values<std::pair<std::string, cbag::purp_t>>({
                {"drawing", 4294967295},
                {"pin", 251},
                {"label", 237},
            }));

        REQUIRE(lay_pair.second == obj.get_layer_id(lay_pair.first));

        REQUIRE(purp_pair.second == obj.get_purpose_id(purp_pair.first));

        REQUIRE(obj.get_purpose_id("drawing") == obj.get_purpose_id(""));

        REQUIRE(cbag::layer_t(lay_pair.second, purp_pair.second) ==
                get_layer_t(obj, lay_pair.first, purp_pair.first));
    }
}
