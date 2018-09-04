#include <algorithm>

#include <cbag/layout/rectangle.h>
#include <cbag/layout/transformation.h>

namespace cbag {
namespace layout {

rectangle::rectangle() : polygon90(pt_vector(0, 0, 0, 0), winding_dir::clockwise_winding) {}

rectangle::rectangle(coord_t xl, coord_t yl, coord_t xh, coord_t yh)
    : polygon90(pt_vector(xl, yl, xh, yh), winding_dir::clockwise_winding) {}

void rectangle::set_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh) {
    data.at_raw(0).set(xl, yl);
    data.at_raw(1).set(xh, yh);
}

rectangle &rectangle::transform(const cbag::layout::transformation &xform) {
    bp::transform(*this, xform);
    return *this;
}

rectangle::interval_type rectangle::get(bp::orientation_2d orient) const {
    if (orient.to_int() == bp::HORIZONTAL) {
        return {data[0].x(), data[1].x()};
    }
    return {data[0].y(), data[1].y()};
}

} // namespace layout
} // namespace cbag
