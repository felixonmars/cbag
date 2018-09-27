
#include <iostream>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <cbag/layout/point_t.h>
#include <cbag/layout/rectangle.h>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// specializations for boost::geometry
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(cbag::layout::point_t, cbag::coord_t, bg::cs::cartesian, x,
                                         y, x, y)

BOOST_GEOMETRY_REGISTER_BOX_2D_4VALUES(cbag::layout::rectangle, cbag::layout::point_t, xl, yl, xh,
                                       yh)

template <typename Value> struct obj_eq {
    bool operator()(Value const &l, Value const &r) const { return l.second == r.second; }
};

void test_rtree() {
    using box = cbag::layout::rectangle;
    using value = std::pair<box, unsigned>;
    using rtree_t = bgi::rtree<value, bgi::quadratic<32, 16>, bgi::indexable<value>, obj_eq<value>>;

    // create the rtree using default constructor
    rtree_t rtree;

    // create some values
    for (unsigned i = 0; i < 20; i += 2) {
        // create a box
        box b(i, i, i + 1, i + 1);
        // insert new value
        rtree.insert(std::make_pair(b, i));
    }
    auto b = rtree.bounds();
    std::cout << "Total bound box: " << bg::wkt<rtree_t::bounds_type>(b) << std::endl;
    // print all boxes
    std::cout << "All boxes:" << std::endl;
    for (auto it = rtree.qbegin(bgi::intersects(b)); it != rtree.qend(); ++it) {
        std::cout << bg::wkt<box>(it->first) << std::endl;
    }

    // remove a box
    value val = std::make_pair(box(0, 0, 0, 0), 6);
    std::cout << "Removing " << bg::wkt<box>(val.first) << std::endl;
    rtree.remove(val);

    // print again
    std::cout << "All boxes:" << std::endl;
    for (auto it = rtree.qbegin(bgi::intersects(rtree.bounds())); it != rtree.qend(); ++it) {
        std::cout << bg::wkt<box>(it->first) << std::endl;
    }
}

int main(int argc, char *argv[]) {
    test_rtree();

    return 0;
}
