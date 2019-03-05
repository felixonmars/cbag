#include <catch2/catch.hpp>

#include <cbag/common/transformation_util.h>
#include <cbag/enum/end_style.h>
#include <cbag/layout/cellview_poly.h>
#include <cbag/layout/cellview_util.h>
#include <cbag/layout/grid_object.h>
#include <cbag/layout/path_util.h>
#include <cbag/layout/routing_grid.h>
#include <cbag/layout/via_wrapper.h>

using c_tech = cbag::layout::tech;
using c_grid = cbag::layout::routing_grid;
using c_cellview = cbag::layout::cellview;
using c_xform = cbag::transformation;
using c_tid = cbag::layout::track_id;
using c_warr = cbag::layout::wire_array;
using c_offset_t = cbag::offset_t;
using c_via_param = cbag::layout::via_param;
using c_box = cbag::box_t;

using pt_type = std::array<cbag::coord_t, 2>;
using intv_type = std::array<cbag::coord_t, 2>;

c_tech make_tech_info() { return c_tech("tests/data/test_layout/tech_params.yaml"); }
std::shared_ptr<const c_grid> make_grid(const c_tech &tech) {
    return std::make_shared<const c_grid>(&tech, "tests/data/test_layout/grid.yaml");
}
std::shared_ptr<c_cellview> make_cv(const std::shared_ptr<const c_grid> &grid) {
    return std::make_shared<c_cellview>(grid, "CBAG_TEST");
}

TEST_CASE("add wire array", "[layout::cellview]") {
    auto tech_info = make_tech_info();
    auto grid = make_grid(tech_info);
    auto cv = make_cv(grid);
    auto[tid, lower, upper] = GENERATE(values<std::tuple<c_tid, c_offset_t, c_offset_t>>({
        // one track
        {c_tid(4, 0, 1, 1, 0), 0, 100},
        {c_tid(4, -2, 2, 1, 0), -200, 100},
        {c_tid(4, 5, 3, 1, 0), -50, 100},
        // two tracks
        {c_tid(4, 0, 1, 2, 1), 0, 100},
        {c_tid(4, -2, 2, 4, -4), -300, -100},
    }));

    auto warr = c_warr(std::make_shared<c_tid>(tid), lower, upper);

    cbag::layout::add_warr(*cv, warr);
}

TEST_CASE("add path", "[layout::cellview]") {
    using data_type = std::tuple<std::array<std::string, 2>, std::vector<pt_type>, cbag::offset_t,
                                 std::array<cbag::end_style, 3>>;

    auto tech_info = make_tech_info();
    auto grid = make_grid(tech_info);
    auto cv = make_cv(grid);
    auto[lay_purp, pt_list, half_w, styles] = GENERATE(values<data_type>({
        {
            {"M2", ""},
            {{0, 0}, {2000, 0}, {3000, 1000}, {3000, 3000}},
            10,
            {cbag::end_style::truncate, cbag::end_style::round, cbag::end_style::round},
        },
    }));

    cv->set_geometry_mode(cbag::geometry_mode::POLY45);
    add_path(cv, lay_purp[0], lay_purp[1], pt_list, half_w, styles[0], styles[1], styles[2], true);

    // create expected geometry
    auto key = cbag::layout::layer_t_at(*(cv->get_tech()), lay_purp[0], lay_purp[1]);
    cbag::layout::geometry expect(cbag::geometry_mode::POLY45);
    auto poly_set = cbag::layout::make_path(pt_list, half_w, styles[0], styles[1], styles[2]);
    expect.add_shape(poly_set);

    auto geo_iter = cv->find_geometry(key);
    REQUIRE(geo_iter != cv->end_geometry());
    REQUIRE(geo_iter->second == expect);
}

TEST_CASE("add blockage", "[layout::cellview]") {
    using data_type = std::tuple<std::string, cbag::blockage_type, std::vector<pt_type>>;

    auto tech_info = make_tech_info();
    auto grid = make_grid(tech_info);
    auto cv = make_cv(grid);
    auto[layer, blk_type, pt_list] = GENERATE(values<data_type>({
        {"", cbag::blockage_type::placement, {{0, 0}, {100, 0}, {0, 100}}},
    }));

    cv->set_geometry_mode(cbag::geometry_mode::POLY45);
    cbag::layout::add_blockage(cv, layer, blk_type, pt_list, true);
}

TEST_CASE("add via", "[layout::cellview]") {
    using data_type = std::tuple<c_xform, std::string, std::string, c_via_param, c_box, c_box>;

    auto tech_info = make_tech_info();
    auto grid = make_grid(tech_info);
    auto cv = make_cv(grid);
    auto[xform, lay1, lay2, via_param, box1, box2] = GENERATE(values<data_type>({
        {cbag::make_xform(0, 0), "M1", "M2",
         c_via_param(1, 1, 32, 32, 0, 0, 14, 14, 14, 14, 14, 14, 14, 14), c_box(-30, -30, 30, 30),
         c_box(-30, -30, 30, 30)},
    }));

    auto l1 = cbag::layout::layer_t_at(tech_info, lay1, "");
    auto l2 = cbag::layout::layer_t_at(tech_info, lay2, "");
    auto via_id = tech_info.get_via_id(cbag::direction::LOWER, l1, l2);

    cv->add_object(cbag::layout::via_wrapper(cbag::layout::via(xform, via_id, via_param), true));

    cbag::layout::geometry expect1(cbag::geometry_mode::POLY90);
    expect1.add_shape(box1);
    cbag::layout::geometry expect2(cbag::geometry_mode::POLY90);
    expect2.add_shape(box2);

    auto iter_end = cv->end_geometry();
    auto iter1 = cv->find_geometry(l1);
    auto iter2 = cv->find_geometry(l2);
    REQUIRE(iter1 != iter_end);
    REQUIRE(iter2 != iter_end);
    REQUIRE(iter1->second == expect1);
    REQUIRE(iter2->second == expect2);
}

TEST_CASE("add via on intersections", "[layout::cellview]") {
    using data_type =
        std::tuple<c_tid, c_tid, intv_type, intv_type, bool, bool, std::array<intv_type, 2>>;

    auto tech_info = make_tech_info();
    auto grid = make_grid(tech_info);
    auto cv = make_cv(grid);
    auto[tid1, tid2, coord1, coord2, extend, contain, expect] = GENERATE(values<data_type>({
        {c_tid(4, 0, 1, 1, 0),
         c_tid(5, 4, 1, 1, 0),
         {cbag::COORD_MIN, cbag::COORD_MAX},
         {cbag::COORD_MIN, cbag::COORD_MAX},
         true,
         false,
         {intv_type{410, 490}, intv_type{30, 90}}},
    }));

    auto ans =
        cbag::layout::add_via_on_intersections(*cv, tid1, tid2, coord1, coord2, extend, contain);

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
    auto[tid1, tid2, coord1, w_ext, tr_ext, expect] = GENERATE(values<data_type>({
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
    auto ans = cbag::layout::connect_warr_track(*cv, warr, tid2, w_ext, tr_ext);

    REQUIRE(ans == expect);
}
