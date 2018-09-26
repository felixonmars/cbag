#include <algorithm>

#include <cbag/layout/rectangle.h>
#include <cbag/layout/transformation.h>

namespace cbag {
namespace layout {

rectangle::rectangle() = default;

rectangle::rectangle(coord_t xl, coord_t yl, coord_t xh, coord_t yh)
    : xl(xl), yl(yl), xh(xh), yh(yh) {}

coord_t rectangle::xm() const { return (xl + xh) / 2; }
coord_t rectangle::ym() const { return (yl + yh) / 2; }
offset_t rectangle::w() const { return xh - xl; }
offset_t rectangle::h() const { return yh - yl; }

bool rectangle::is_physical() const { return xh > xl && yh > yl; }
bool rectangle::is_valid() const { return xh >= xl && yh >= yl; }
bool rectangle::overlaps(const rectangle &r) const { return bp::intersects(*this, r, false); }

rectangle rectangle::get_merge(const rectangle &other) const {
    if (!is_physical())
        return other;
    if (!other.is_physical())
        return *this;

    return {std::min(xl, other.xl), std::min(yl, other.yl), std::max(xh, other.xh),
            std::max(yh, other.yh)};
}

rectangle rectangle::get_intersect(const rectangle &other) const {
    return {std::max(xl, other.xl), std::max(yl, other.yl), std::min(xh, other.xh),
            std::min(yh, other.yh)};
}

rectangle rectangle::get_extend_to(coord_t x, coord_t y) const {
    return {std::min(xl, x), std::min(yl, y), std::max(xh, x), std::max(yh, y)};
}

rectangle rectangle::get_transform(const transformation &xform) const {
    rectangle ans = *this;
    return bp::transform(ans, xform);
}

rectangle::interval_type rectangle::get(bp::orientation_2d orient) const {
    if (orient.to_int() == bp::HORIZONTAL) {
        return {xl, xh};
    }
    return {yl, yh};
}

} // namespace layout
} // namespace cbag
