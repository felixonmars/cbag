#include <catch2/catch.hpp>

#include <cbag/layout/path_util.h>

using pt_type = std::array<cbag::coord_t, 2>;

TEST_CASE("make path", "[layout::cellview]") {
    using data_type = std::tuple<std::vector<pt_type>, cbag::offset_t,
                                 std::array<cbag::end_style, 3>, std::vector<pt_type>>;

    auto [pt_list, half_w, styles, expect] = GENERATE(values<data_type>({
        {{{0, 0}, {2000, 0}, {3000, 1000}, {3000, 3000}},
         10,
         {cbag::end_style::truncate, cbag::end_style::round, cbag::end_style::round},
         {{3010, 3004},
          {3004, 3010},
          {2996, 3010},
          {2990, 3004},
          {2990, 1004},
          {1996, 10},
          {0, 10},
          {0, -10},
          {2004, -10},
          {3010, 996},
          {3010, 3004}}},

    }));

    auto poly = cbag::layout::make_path(pt_list, half_w, styles[0], styles[1], styles[2]);

    std::vector<cbag::layout::polygon45> poly_vec;
    std::vector<pt_type> pt_vec;
    poly.get_polygons(poly_vec);

    REQUIRE(poly_vec.size() == 1);

    for (const auto &pt : poly_vec[0]) {
        pt_vec.push_back(pt_type{pt.x(), pt.y()});
    }
    REQUIRE(pt_vec == expect);
}
