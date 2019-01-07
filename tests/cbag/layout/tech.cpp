#include <catch2/catch.hpp>

#include <cbag/enum/space_type.h>
#include <cbag/layout/tech_util.h>

using c_tech = cbag::layout::tech;

TEST_CASE("technology layer/purpose lookup", "[tech]") {
    c_tech obj("tests/data/test_layout/tech_params.yaml");

    auto [lay_name, lay_id] = GENERATE(values<std::pair<std::string, cbag::lay_t>>({
        {"M1", 60},
        {"M2", 80},
        {"M3", 100},
        {"M4", 110},
    }));

    auto [purp_name, purp_id] = GENERATE(values<std::pair<std::string, cbag::purp_t>>({
        {"drawing", 4294967295},
        {"pin", 251},
        {"label", 237},
    }));

    REQUIRE(lay_id == cbag::layout::layer_id_at(obj, lay_name));
    REQUIRE(purp_id == cbag::layout::purpose_id_at(obj, purp_name));
    REQUIRE(obj.get_purpose_id("drawing") == cbag::layout::purpose_id_at(obj, ""));
    REQUIRE(cbag::layer_t(lay_id, purp_id) == cbag::layout::layer_t_at(obj, lay_name, purp_name));
}

TEST_CASE("technology get_min_space", "[tech]") {
    c_tech obj("tests/data/test_layout/tech_params.yaml");

    auto [sp_type, lay_name, w, min_sp] =
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

    auto key = layer_t_at(obj, lay_name, "drawing");
    REQUIRE(obj.get_min_space(key, w, sp_type, false) == min_sp);
}

TEST_CASE("technology get_min_length", "[tech]") {
    c_tech obj("tests/data/test_layout/tech_params.yaml");

    auto [lay_name, w, min_len, even] =
        GENERATE(values<std::tuple<std::string, cbag::offset_t, cbag::offset_t, bool>>({
            {"M1", 1, 6176, true},
            {"M1", 10, 618, true},
            {"M1", 20, 310, true},
            {"M1", 20, 309, false},
            {"M1", 40, 156, true},
            {"M1", 40, 155, false},
            {"M1", 50, 124, true},
            {"M1", 50, 124, false},
            {"M1", 800, 10, true},
            {"M1", 800, 10, false},
        }));

    auto key = layer_t_at(obj, lay_name, "drawing");
    REQUIRE(obj.get_min_length(key, w, even) == min_len);
}
