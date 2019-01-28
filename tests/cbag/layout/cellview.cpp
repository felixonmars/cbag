#include <catch2/catch.hpp>

#include <cbag/enum/end_style.h>
#include <cbag/layout/cellview_poly.h>
#include <cbag/layout/cellview_util.h>
#include <cbag/layout/grid_object.h>
#include <cbag/layout/path_util.h>
#include <cbag/layout/routing_grid.h>

using c_tech = cbag::layout::tech;
using c_grid = cbag::layout::routing_grid;
using c_cellview = cbag::layout::cellview;
using c_tid = cbag::layout::track_id;
using c_warr = cbag::layout::wire_array;
using c_offset_t = cbag::offset_t;

using pt_type = std::array<cbag::coord_t, 2>;
using intv_type = std::array<cbag::coord_t, 2>;

c_tech make_tech_info() { return c_tech("tests/data/test_layout/tech_params.yaml"); }
c_grid make_grid(const c_tech &tech) { return c_grid(&tech, "tests/data/test_layout/grid.yaml"); }
c_cellview make_cv(const c_grid &grid) { return c_cellview(&grid, "CBAG_TEST"); }

TEST_CASE("add wire array", "[layout::cellview]") {
    auto tech_info = make_tech_info();
    auto grid = make_grid(tech_info);
    auto cv = make_cv(grid);
    auto [tid, lower, upper] = GENERATE(values<std::tuple<c_tid, c_offset_t, c_offset_t>>({
        // one track
        {c_tid(4, 0, 1, 1, 0), 0, 100},
        {c_tid(4, -2, 2, 1, 0), -200, 100},
        {c_tid(4, 5, 3, 1, 0), -50, 100},
        // two tracks
        {c_tid(4, 0, 1, 2, 1), 0, 100},
        {c_tid(4, -2, 2, 4, -4), -300, -100},
    }));

    auto warr = c_warr(std::make_shared<c_tid>(tid), lower, upper);

    cbag::layout::add_warr(cv, warr);
}

TEST_CASE("add path", "[layout::cellview]") {
    using data_type = std::tuple<std::array<std::string, 2>, std::vector<pt_type>, cbag::offset_t,
                                 std::array<cbag::end_style, 3>>;

    auto tech_info = make_tech_info();
    auto grid = make_grid(tech_info);
    auto cv = make_cv(grid);
    auto [lay_purp, pt_list, half_w, styles] = GENERATE(values<data_type>({
        {
            {"M2", ""},
            {{0, 0}, {2000, 0}, {3000, 1000}, {3000, 3000}},
            10,
            {cbag::end_style::truncate, cbag::end_style::round, cbag::end_style::round},
        },
    }));

    cv.set_geometry_mode(cbag::geometry_mode::POLY45);
    add_path(cv, lay_purp[0], lay_purp[1], pt_list, half_w, styles[0], styles[1], styles[2], true);

    // create expected geometry
    auto key = cbag::layout::layer_t_at(*cv.get_tech(), lay_purp[0], lay_purp[1]);
    cbag::layout::geometry expect(cbag::geometry_mode::POLY45);
    auto poly_set = cbag::layout::make_path(pt_list, half_w, styles[0], styles[1], styles[2]);
    expect.add_shape(poly_set);

    auto geo_iter = cv.find_geometry(key);
    REQUIRE(geo_iter != cv.end_geometry());
    REQUIRE(geo_iter->second == expect);
}

TEST_CASE("add blockage", "[layout::cellview]") {
    using data_type = std::tuple<std::string, cbag::blockage_type, std::vector<pt_type>>;

    auto tech_info = make_tech_info();
    auto grid = make_grid(tech_info);
    auto cv = make_cv(grid);
    auto [layer, blk_type, pt_list] = GENERATE(values<data_type>({
        {"", cbag::blockage_type::placement, {{0, 0}, {100, 0}, {0, 100}}},
    }));

    cv.set_geometry_mode(cbag::geometry_mode::POLY45);
    cbag::layout::add_blockage(cv, layer, blk_type, pt_list, true);
}

TEST_CASE("add via on intersections", "[layout::cellview]") {
    using data_type =
        std::tuple<c_tid, c_tid, intv_type, intv_type, bool, bool, std::array<intv_type, 2>>;

    auto tech_info = make_tech_info();
    auto grid = make_grid(tech_info);
    auto cv = make_cv(grid);
    auto [tid1, tid2, coord1, coord2, extend, contain, expect] = GENERATE(values<data_type>({
        {c_tid(4, 0, 1, 1, 0),
         c_tid(5, 4, 1, 1, 0),
         {cbag::COORD_MIN, cbag::COORD_MAX},
         {cbag::COORD_MIN, cbag::COORD_MAX},
         true,
         false,
         {intv_type{410, 490}, intv_type{30, 90}}},
    }));

    auto ans =
        cbag::layout::add_via_on_intersections(cv, tid1, tid2, coord1, coord2, extend, contain);

    REQUIRE(ans == expect);
}

TEST_CASE("connect warr to track", "[layout::cellview]") {
    using ext_type = std::optional<cbag::coord_t>;
    using ext_arr_type = std::array<ext_type, 2>;
    using data_type =
        std::tuple<c_tid, c_tid, intv_type, ext_arr_type, ext_arr_type, std::array<intv_type, 2>>;

    auto tech_info = make_tech_info();
    auto grid = make_grid(tech_info);
    auto cv = make_cv(grid);
    auto [tid1, tid2, coord1, w_ext, tr_ext, expect] = GENERATE(values<data_type>({
        {c_tid(4, 0, 1, 1, 0),
         c_tid(5, 4, 1, 1, 0),
         {0, 400},
         {ext_type{}, ext_type{}},
         {ext_type{}, ext_type{}},
         {intv_type{0, 490}, intv_type{30, 90}}},
        {c_tid(4, 0, 1, 1, 0),
         c_tid(5, 4, 1, 1, 0),
         {0, 400},
         {ext_type{}, ext_type{}},
         {0, 400},
         {intv_type{0, 490}, intv_type{0, 400}}},
    }));

    auto warr = c_warr(std::make_shared<c_tid>(tid1), coord1[0], coord1[1]);
    auto ans = cbag::layout::connect_warr_track(cv, warr, tid2, w_ext, tr_ext);

    REQUIRE(ans == expect);
}
