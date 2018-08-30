#include <cbag/layout/rectangle.h>

namespace cbag {
namespace layout {

rectangle::rectangle()
    : polygon90({point_t(0, 0), point_t(0, 0)}, winding_dir::clockwise_winding) {}

rectangle::rectangle(coord_t xl, coord_t yl, coord_t xh, coord_t yh)
    : polygon90({point_t(xl, yl), point_t(xh, yh)}, winding_dir::clockwise_winding) {}

rectangle::interval_type rectangle::get(bp::orientation_2d orient) const {
    if (orient.to_int() == bp::HORIZONTAL) {
        return {data[0].x(), data[1].x()};
    }
    return {data[0].y(), data[1].y()};
}

} // namespace layout
} // namespace cbag
