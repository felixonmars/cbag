/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_RECTANGLE_H
#define CBAG_LAYOUT_RECTANGLE_H

#include <utility>
#include <variant>
#include <vector>

#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>

namespace bp = boost::polygon;

namespace cbag {
namespace layout {

class transformation;

class rectangle {
  public:
    using coordinate_type = coord_t;
    using interval_type = bp::interval_data<coord_t>;

    coord_t xl = 0, yl = 0, xh = 0, yh = 0;

    rectangle();

    rectangle(coord_t xl, coord_t yl, coord_t xh, coord_t yh);

    template <typename T1, typename T2>
    rectangle(const T1 &hrange, const T2 &vrange)
        : xl(bp::low(hrange)), yl(bp::low(vrange)), xh(bp::high(hrange)), yh(bp::high(vrange)) {}

    coord_t xm() const;
    coord_t ym() const;
    offset_t w() const;
    offset_t h() const;

    bool is_physical() const;
    bool is_valid() const;
    bool overlaps(const rectangle &r) const;
    bool operator==(const rectangle &r) const;

    rectangle get_merge(const rectangle &other) const;
    rectangle get_intersect(const rectangle &other) const;
    rectangle get_extend_to(coord_t x, coord_t y) const;
    rectangle get_transform(const transformation &xform) const;

    interval_type get(bp::orientation_2d orient) const;

    rectangle &merge(const rectangle &other);
    rectangle &transform(const transformation &xform);

    template <typename T2> void set(bp::orientation_2d orient, const T2 &interval) {
        if (orient.to_int() == bp::orientation_2d_enum::HORIZONTAL) {
            xl = bp::low(interval);
            xh = bp::high(interval);
        } else {
            yl = bp::low(interval);
            yh = bp::high(interval);
        }
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
