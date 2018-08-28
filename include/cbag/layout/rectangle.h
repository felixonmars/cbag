/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_RECTANGLE_H
#define CBAG_LAYOUT_RECTANGLE_H

#include <cbag/layout/polygon90.h>

namespace cbag {
namespace layout {

// -----------------------------------------------------------------------------
// rectangle declaration
// for rectangle, the first point is the lower left vertex, the second point
// is the upper right vertex.
// as the result, rectangle always have exactly 2 points in data.
// -----------------------------------------------------------------------------

class rectangle : public polygon90 {
  public:
    using interval_type = bp::interval_data<coordinate_type>;

    // defaults to 0 area rectangle at origin
    inline rectangle() : polygon90({point_t(0, 0), point_t(0, 0)}) {}
    inline rectangle(coord_t xl, coord_t yl, coord_t xh, coord_t yh)
        : polygon90({point_t(xl, yl), point_t(xh, yh)}) {}

    template <typename T1, typename T2>
    inline rectangle(const T1 &hrange, const T2 &vrange)
        : polygon90({point_t(0, 0), point_t(0, 0)}) {
        set(bp::HORIZONTAL, hrange);
        set(bp::VERTICAL, vrange);
    }

    inline interval_type get(bp::orientation_2d orient) const {
        if (orient.to_int() == bp::HORIZONTAL) {
            return {data[0].x(), data[1].x()};
        }
        return {data[0].y(), data[1].y()};
    }

    template <typename T2> inline void set(bp::orientation_2d orient, const T2 &interval) {
        data[0].set(orient, bp::low(interval));
        data[1].set(orient, bp::high(interval));
    }
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::rectangle> { using type = rectangle_concept; };

} // namespace polygon
} // namespace boost

#endif
