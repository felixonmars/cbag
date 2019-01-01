#include <catch2/catch.hpp>

#include <cbag/common/vector.h>
#include <cbag/enum/orient_2d.h>
#include <cbag/layout/tech_util.h>

using c_tech = cbag::layout::tech;
using c_vector = cbag::vector;

TEST_CASE("get_via_param() for 1 via", "[via]") {
    c_tech obj("tests/data/test_layout/tech_params.yaml");
    auto bot_layer = layer_t_at(obj, "M1", "drawing");
    auto top_layer = layer_t_at(obj, "M2", "drawing");
    auto bot_dir = cbag::orient_2d::HORIZONTAL;
    auto top_dir = cbag::orient_2d::VERTICAL;
    auto extend = true;

    cbag::layout::via_param param;

    // test square via
    param = obj.get_via_param(c_vector{32, 32}, bot_layer, top_layer, bot_dir, top_dir, extend);
    REQUIRE(param.num == std::array<cbag::cnt_t, 2>{1, 1});
    REQUIRE(param.cut_dim == c_vector{32, 32});
    REQUIRE(param.cut_spacing == c_vector{0, 0});
    REQUIRE(param.enc == std::array<c_vector, 2>{c_vector{40, 0}, c_vector{0, 40}});
    REQUIRE(param.off == std::array<c_vector, 2>{c_vector{0, 0}, c_vector{0, 0}});

    // test square via constraint switch works properly
    param = obj.get_via_param(c_vector{34, 34}, bot_layer, top_layer, bot_dir, top_dir, extend);
    REQUIRE(param.num == std::array<cbag::cnt_t, 2>{1, 1});
    REQUIRE(param.cut_dim == c_vector{32, 32});
    REQUIRE(param.cut_spacing == c_vector{0, 0});
    REQUIRE(param.enc == std::array<c_vector, 2>{c_vector{40, 1}, c_vector{1, 40}});
    REQUIRE(param.off == std::array<c_vector, 2>{c_vector{0, 0}, c_vector{0, 0}});

    param = obj.get_via_param(c_vector{36, 36}, bot_layer, top_layer, bot_dir, top_dir, extend);
    REQUIRE(param.num == std::array<cbag::cnt_t, 2>{1, 1});
    REQUIRE(param.cut_dim == c_vector{32, 32});
    REQUIRE(param.cut_spacing == c_vector{0, 0});
    REQUIRE(param.enc == std::array<c_vector, 2>{c_vector{34, 2}, c_vector{2, 34}});
    REQUIRE(param.off == std::array<c_vector, 2>{c_vector{0, 0}, c_vector{0, 0}});

    // test hrect via
    param = obj.get_via_param(c_vector{84, 32}, bot_layer, top_layer, bot_dir, top_dir, extend);
    REQUIRE(param.num == std::array<cbag::cnt_t, 2>{1, 1});
    REQUIRE(param.cut_dim == c_vector{64, 32});
    REQUIRE(param.cut_spacing == c_vector{0, 0});
    REQUIRE(param.enc == std::array<c_vector, 2>{c_vector{20, 0}, c_vector{10, 10}});
    REQUIRE(param.off == std::array<c_vector, 2>{c_vector{0, 0}, c_vector{0, 0}});

    // test vrect via
    param = obj.get_via_param(c_vector{32, 84}, bot_layer, top_layer, bot_dir, top_dir, extend);
    REQUIRE(param.num == std::array<cbag::cnt_t, 2>{1, 1});
    REQUIRE(param.cut_dim == c_vector{32, 64});
    REQUIRE(param.cut_spacing == c_vector{0, 0});
    REQUIRE(param.enc == std::array<c_vector, 2>{c_vector{10, 10}, c_vector{0, 20}});
    REQUIRE(param.off == std::array<c_vector, 2>{c_vector{0, 0}, c_vector{0, 0}});
}
