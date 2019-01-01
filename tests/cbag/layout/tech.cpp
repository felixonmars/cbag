#include <catch2/catch.hpp>

#include <cbag/enum/space_type.h>
#include <cbag/layout/tech_util.h>

using c_tech = cbag::layout::tech;

TEST_CASE("technology layer/purpose lookup", "[tech]") {
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

    REQUIRE(lay_pair.second == cbag::layout::layer_id_at(obj, lay_pair.first));

    REQUIRE(purp_pair.second == cbag::layout::purpose_id_at(obj, purp_pair.first));

    REQUIRE(obj.get_purpose_id("drawing") == cbag::layout::purpose_id_at(obj, ""));

    REQUIRE(cbag::layer_t(lay_pair.second, purp_pair.second) ==
            cbag::layout::layer_t_at(obj, lay_pair.first, purp_pair.first));
}

TEST_CASE("technology get_min_space", "[tech]") {
    c_tech obj("tests/data/test_layout/tech_params.yaml");

    auto args =
        GENERATE(values<std::tuple<cbag::space_type, std::string, cbag::offset_t, cbag::offset_t>>({
            {cbag::space_type::DIFF_COLOR, "M1", 10, 32},
            {cbag::space_type::DIFF_COLOR, "M2", 100, 32},
            {cbag::space_type::DIFF_COLOR, "M4", 10, 48},
            {cbag::space_type::DIFF_COLOR, "M4", 99, 48},
            {cbag::space_type::DIFF_COLOR, "M4", 100, 72},
            {cbag::space_type::DIFF_COLOR, "M4", 9999, 220},
            {cbag::space_type::SAME_COLOR, "M5", 89, 80},
            {cbag::space_type::SAME_COLOR, "M6", 90, 100},
        }));

    auto key = layer_t_at(obj, std::get<1>(args), "drawing");
    auto ans = obj.get_min_space(key, std::get<2>(args), std::get<0>(args));
    REQUIRE(ans == std::get<3>(args));
}
