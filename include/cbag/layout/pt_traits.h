#ifndef CBAG_LAYOUT_PT_TRAITS_H
#define CBAG_LAYOUT_PT_TRAITS_H

#include <boost/polygon/polygon.hpp>

#include <cbag/common/point.h>

namespace boost {
namespace polygon {
template <> struct geometry_concept<cbag::point> { using type = point_concept; };

template <> struct point_traits<cbag::point> {
    using point_type = cbag::point;
    using coordinate_type = cbag::coord_t;

    static coordinate_type get(const point_type &point, orientation_2d orient) {
        return (orient.to_int() == static_cast<unsigned int>(orientation_2d_enum::HORIZONTAL))
                   ? point.first
                   : point.second;
    }
};
} // namespace polygon
} // namespace boost

#endif
