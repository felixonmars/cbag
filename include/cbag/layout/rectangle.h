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

class transformation;

class rectangle : public polygon90 {
  public:
    using interval_type = bp::interval_data<coordinate_type>;

    rectangle();

    rectangle(coord_t xl, coord_t yl, coord_t xh, coord_t yh);

    template <typename T1, typename T2>
    rectangle(const T1 &hrange, const T2 &vrange)
        : polygon90(pt_vector(bp::low(hrange), bp::low(vrange), bp::high(hrange), bp::high(vrange)),
                    winding_dir::clockwise_winding) {}

    void set_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh);

    rectangle &transform(const transformation &xform);

    coord_t xl() const { return data.at_raw(0).x(); }
    coord_t yl() const { return data.at_raw(0).y(); }
    coord_t xh() const { return data.at_raw(1).x(); }
    coord_t yh() const { return data.at_raw(1).y(); }
    coord_t xm() const { return (xh() + xl()) / 2; }
    coord_t ym() const { return (yh() + yl()) / 2; }
    offset_t w() const { return xh() - xl(); }
    offset_t h() const { return yh() - yl(); }
    bool is_valid() const { return xh() >= xl() && yh() >= yl(); }
    bool is_physical() const { return xh() > xl() && yh() > yl(); }
    // methods for boost::polygon

    interval_type get(bp::orientation_2d orient) const;

    template <typename T2> void set(bp::orientation_2d orient, const T2 &interval) {
        if (orient.to_int() == bp::orientation_2d_enum::HORIZONTAL) {
            data[0][0] = bp::low(interval);
            data[1][0] = bp::high(interval);
        } else {
            data[0][1] = bp::low(interval);
            data[1][1] = bp::high(interval);
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
