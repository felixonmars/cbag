#include <cbag/layout/rectangle.h>
#include <cbag/layout/transformation.h>

namespace cbag {
namespace layout {

rectangle::rectangle() = default;

rectangle::rectangle(coord_t xl, coord_t yl, coord_t xh, coord_t yh)
    : xl(xl), yl(yl), xh(xh), yh(yh) {}

void rectangle::set_rect(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
    xl = x0;
    yl = y0;
    xh = x1;
    yh = y1;
}

rectangle &rectangle::transform(const transformation &xform) {
    bp::transform(*this, xform);
    return *this;
}

coord_t rectangle::xm() const { return (xl + xh) / 2; }
coord_t rectangle::ym() const { return (yl + yh) / 2; }
offset_t rectangle::w() const { return xh - xl; }
offset_t rectangle::h() const { return yh - yl; }

bool rectangle::is_physical() const { return xh > xl && yh > yl; }

rectangle::interval_type rectangle::get(bp::orientation_2d orient) const {
    if (orient.to_int() == bp::HORIZONTAL) {
        return {xl, xh};
    }
    return {yl, yh};
}

} // namespace layout
} // namespace cbag
