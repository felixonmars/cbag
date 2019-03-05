#include <catch2/catch.hpp>

#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech.h>

using c_tech = cbag::layout::tech;
using c_grid = cbag::layout::routing_grid;
using c_offset_t = cbag::offset_t;

TEST_CASE("track pitch and block pitch work properly", "[grid]") {
    auto tech_info = std::make_shared<c_tech>("tests/data/test_layout/tech_params.yaml");
    c_grid grid(tech_info, "tests/data/test_layout/grid.yaml");

    auto[lev, tp, bp, bp2] =
        GENERATE(values<std::tuple<cbag::level_t, c_offset_t, c_offset_t, c_offset_t>>({
            {1, 120, 120, 60},
            {2, 120, 120, 60},
            {3, 120, 120, 60},
            {4, 120, 120, 60},
            {5, 180, 360, 180},
            {6, 180, 360, 180},
            {7, 180, 360, 180},
            {8, 360, 360, 180},
        }));

    REQUIRE(grid[lev].get_pitch() == tp);
    REQUIRE(grid[lev].get_blk_pitch(false) == bp);
    REQUIRE(grid[lev].get_blk_pitch(true) == bp2);
}

TEST_CASE("get_top_track_pitches()", "[grid]") {
    auto tech_info = std::make_shared<c_tech>("tests/data/test_layout/tech_params.yaml");
    c_grid grid(tech_info, "tests/data/test_layout/grid.yaml");

    auto top_layer = 5;
    auto x_pitch_expect = 180;
    auto y_pitch_expect = 120;

    auto[x_pitch, y_pitch] = get_top_track_pitches(grid, top_layer);

    REQUIRE(x_pitch == x_pitch_expect);
    REQUIRE(y_pitch == y_pitch_expect);
}
