
#include <iostream>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

#include <cbag/layout/point_t.h>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

namespace boost {
namespace geometry {
namespace traits {

template <> struct tag<cbag::layout::point_t> { typedef point_tag type; };

template <> struct coordinate_type<cbag::layout::point_t> { typedef cbag::coord_t type; };

template <> struct coordinate_system<cbag::layout::point_t> { typedef cs::cartesian type; };

template <> struct dimension<cbag::layout::point_t> : boost::mpl::int_<2> {};

template <> struct access<cbag::layout::point_t, 0> {
    static cbag::coord_t get(cbag::layout::point_t const &p) { return p.x(); }

    static void set(cbag::layout::point_t &p, cbag::coord_t const &value) { p[0] = value; }
};

template <> struct access<cbag::layout::point_t, 1> {
    static cbag::coord_t get(cbag::layout::point_t const &p) { return p.y(); }

    static void set(cbag::layout::point_t &p, cbag::coord_t const &value) { p[1] = value; }
};
} // namespace traits
} // namespace geometry
} // namespace boost

void test_rtree() {
    typedef cbag::layout::point_t point;
    typedef bg::model::box<point> box;
    typedef std::pair<box, unsigned> value;

    // create the rtree using default constructor
    bgi::rtree<value, bgi::quadratic<16>> rtree;

    // create some values
    for (unsigned i = 0; i < 20; i += 2) {
        // create a box
        box b(point::create(i, i), point::create(i + 1, i + 1));
        // insert new value
        rtree.insert(std::make_pair(b, i));
    }

    // print all boxes
    std::cout << "All boxes:" << std::endl;
    for (auto it = rtree.qbegin(bgi::intersects(rtree.bounds())); it != rtree.qend(); ++it) {
        std::cout << bg::wkt<box>(it->first) << std::endl;
    }

    // remove a box
    value val = std::make_pair(box(point::create(6, 6), point::create(7, 7)), 6);
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
