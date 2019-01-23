#include <catch2/catch.hpp>

#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech.h>

using c_tech = cbag::layout::tech;
using c_grid = cbag::layout::routing_grid;
using c_offset_t = cbag::offset_t;

TEST_CASE("get_top_track_pitches()", "[grid]") {
    c_tech tech_info("tests/data/test_layout/tech_params.yaml");
    c_grid grid(&tech_info, "tests/data/test_layout/grid.yaml");

    auto top_layer = 5;
    auto x_pitch_expect = 180;
    auto y_pitch_expect = 120;

    auto [x_pitch, y_pitch] = get_top_track_pitches(grid, top_layer);

    REQUIRE(x_pitch == x_pitch_expect);
    REQUIRE(y_pitch == y_pitch_expect);
}
