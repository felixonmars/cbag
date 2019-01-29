#include <catch2/catch.hpp>

#include <cbag/common/transformation_util.h>
#include <cbag/common/vector.h>
#include <cbag/enum/direction.h>
#include <cbag/enum/orient_2d.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/via_param.h>
#include <cbag/layout/via_util.h>

using c_tech = cbag::layout::tech;
using c_vector = cbag::vector;
using c_via_param = cbag::layout::via_param;
using c_box = cbag::box_t;
using c_xform = cbag::transformation;

TEST_CASE("get_via_param()", "[via]") {
    c_tech obj("tests/data/test_layout/tech_params.yaml");
    auto bot_dir = cbag::orient_2d::HORIZONTAL;
    auto top_dir = cbag::orient_2d::VERTICAL;
    auto extend = true;

    auto [bl, tl, dim,
          ans] = GENERATE(values<std::tuple<std::string, std::string, c_vector, c_via_param>>({
        // 1 via solutions
        {"M1", "M2", c_vector{32, 32}, c_via_param(1, 1, 32, 32, 0, 0, 40, 40, 0, 0, 0, 0, 40, 40)},
        {"M1", "M2", c_vector{34, 34}, c_via_param(1, 1, 32, 32, 0, 0, 40, 40, 1, 1, 1, 1, 40, 40)},
        {"M1", "M2", c_vector{36, 36}, c_via_param(1, 1, 32, 32, 0, 0, 34, 34, 2, 2, 2, 2, 34, 34)},
        {"M1", "M2", c_vector{84, 32},
         c_via_param(1, 1, 64, 32, 0, 0, 20, 20, 0, 0, 10, 10, 10, 10)},
        {"M1", "M2", c_vector{32, 84},
         c_via_param(1, 1, 32, 64, 0, 0, 10, 10, 10, 10, 0, 0, 20, 20)},
        {"TEST1", "TEST2", c_vector{100, 100},
         c_via_param(1, 1, 10, 10, 0, 0, 45, 45, 45, 45, 45, 45, 45, 45)},
        // 0 via solutions
        {"M1", "M2", c_vector{32, 31}, c_via_param(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},
        {"M1", "M2", c_vector{31, 32}, c_via_param(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},
        {"M1", "M2", c_vector{31, 31}, c_via_param(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},
        {"TEST1", "TEST2", c_vector{20, 20}, c_via_param(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)},
    }));

    auto param =
        obj.get_via_param(dim,
                          obj.get_via_id(cbag::direction::LOWER, layer_t_at(obj, bl, "drawing"),
                                         layer_t_at(obj, tl, "drawing")),
                          cbag::direction::LOWER, bot_dir, top_dir, extend);
    REQUIRE(param == ans);
}

TEST_CASE("get via cuts", "[via]") {
    using data_type = std::tuple<c_xform, std::string, c_via_param, std::vector<c_box>>;

    auto [xform, via_id, via_param, expect] = GENERATE(values<data_type>({
        {cbag::make_xform(0, 0), "M2_M1",
         c_via_param(1, 1, 32, 32, 0, 0, 14, 14, 14, 14, 14, 14, 14, 14),
         std::vector<c_box>{c_box(-16, -16, 16, 16)}},
    }));

    std::vector<c_box> ans;
    auto via = cbag::layout::via(xform, via_id, via_param);
    cbag::layout::get_via_cuts(via, std::back_inserter(ans));
    REQUIRE(ans == expect);
}
