#include <catch2/catch.hpp>

#include <cbag/common/transformation_util.h>

TEST_CASE("transformation concatenation", "[transformation]") {
    using data_type = std::tuple<cbag::transformation, cbag::transformation, cbag::transformation>;
    auto [xform1, xform2, expect] = GENERATE(values<data_type>({
        {cbag::make_xform(), cbag::make_xform(0, 0, cbag::oR90),
         cbag::make_xform(0, 0, cbag::oR90)},
        {cbag::make_xform(3, 5), cbag::make_xform(-10, 2), cbag::make_xform(-7, 7)},
        {cbag::make_xform(3, 5), cbag::make_xform(0, 0, cbag::oR90),
         cbag::make_xform(-5, 3, cbag::oR90)},
    }));

    auto ans = cbag::get_transform_by(xform1, xform2);
    REQUIRE(ans == expect);
}
