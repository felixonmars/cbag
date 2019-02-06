#include <catch2/catch.hpp>

#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation_util.h>

TEST_CASE("transform of invalid box is still invalid", "[box_t]") {
    using data_type = std::tuple<cbag::transformation, cbag::box_t>;
    auto[xform, box] = GENERATE(values<data_type>({
        {cbag::make_xform(), cbag::box_t(0, 0, -1, -1)},
        {cbag::make_xform(5, 5), cbag::box_t(0, 0, -1, -1)},
        {cbag::make_xform(-5, 5, cbag::oR90), cbag::box_t(0, 0, -1, -1)},
    }));

    auto ans = cbag::get_transform(box, xform);
    REQUIRE(!cbag::is_valid(ans));
}
