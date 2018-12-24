
#ifndef CBAG_COMMON_BOX_T_ADAPT_H
#define CBAG_COMMON_BOX_T_ADAPT_H

#include <boost/polygon/rectangle_concept.hpp>
#include <boost/polygon/rectangle_data.hpp>
#include <boost/polygon/rectangle_traits.hpp>

#include <cbag/common/box_t.h>

namespace bp = boost::polygon;

namespace boost {
namespace polygon {

template <> struct geometry_concept<std::array<cbag::coord_t, 2>> {
    using type = interval_concept;
};

template <> struct interval_traits<std::array<cbag::coord_t, 2>> {
    using coordinate_type = cbag::coord_t;

    static coordinate_type get(const std::array<cbag::coord_t, 2> &interval, direction_1d dir) {
        return interval[dir.to_int()];
    }
};

template <> struct interval_mutable_traits<std::array<cbag::coord_t, 2>> {
    using coordinate_type = cbag::coord_t;

    static void set(std::array<cbag::coord_t, 2> &interval, direction_1d dir,
                    coordinate_type value) {
        interval[dir.to_int()] = value;
    }
    static std::array<cbag::coord_t, 2> construct(coordinate_type low_value,
                                                  coordinate_type high_value) {
        return {low_value, high_value};
    }
};

template <> struct geometry_concept<cbag::box_t> { using type = rectangle_concept; };

template <> struct rectangle_traits<cbag::box_t> {
    using coordinate_type = cbag::coord_t;
    using interval_type = std::array<cbag::coord_t, 2>;
    static interval_type get(const cbag::box_t &rectangle, orientation_2d orient) {
        return rectangle.intvs[orient.to_int()];
    }
};

template <> struct rectangle_mutable_traits<cbag::box_t> {
    using coordinate_type = cbag::coord_t;
    using interval_type = std::array<cbag::coord_t, 2>;
    static inline void set(cbag::box_t &rectangle, orientation_2d orient,
                           const interval_type &interval) {
        rectangle.intvs[orient.to_int()] = interval;
    }
    static inline cbag::box_t construct(const interval_type &interval_horizontal,
                                        const interval_type &interval_vertical) {
        return {interval_horizontal[0], interval_vertical[0], interval_horizontal[1],
                interval_vertical[1]};
    }
    static inline cbag::box_t construct(const bp::interval_data<cbag::coord_t> &interval_horizontal,
                                        const bp::interval_data<cbag::coord_t> &interval_vertical) {
        return {interval_horizontal.low(), interval_vertical.low(), interval_horizontal.high(),
                interval_vertical.high()};
    }
};

} // namespace polygon
} // namespace boost

#endif
