
#include <algorithm>

#include <cbag/common/box_t.h>
#include <cbag/common/transformation.h>

namespace cbag {

box_t::box_t() : intvs{{0, 0}, {0, 0}} {}

box_t::box_t(interval_type horizontal, interval_type vertical)
    : intvs{std::move(horizontal), std::move(vertical)} {}

box_t::box_t(coord_t xl, coord_t yl, coord_t xh, coord_t yh) : intvs{{xl, xh}, {yl, yh}} {}

coord_t box_t::xl() const { return intvs[0][0]; }
coord_t box_t::yl() const { return intvs[1][0]; }
coord_t box_t::xh() const { return intvs[0][1]; }
coord_t box_t::yh() const { return intvs[1][1]; }
coord_t box_t::xm() const { return bp::center(intvs[0]); }
coord_t box_t::ym() const { return bp::center(intvs[1]); }
coord_t box_t::w() const { return xh() - yl(); }
coord_t box_t::h() const { return yh() - yl(); }

bool box_t::is_physical() const { return w() > 0 && h() > 0; }
bool box_t::is_valid() const { return w() >= 0 && h() >= 0; }
bool box_t::overlaps(const box_t &other) const { return bp::intersects(*this, other, true); }
const box_t::interval_type &box_t::get_interval(bp::orientation_2d_enum orient) const {
    return intvs[static_cast<unsigned int>(orient)];
}

box_t box_t::get_merge(const box_t &other) const {
    box_t ans(*this);
    bp::encompass(ans, other);
    return ans;
}

box_t box_t::get_intersect(const box_t &other) const {
    return {std::max(xl(), other.xl()), std::max(yl(), other.yl()), std::min(xh(), other.xh()),
            std::min(yh(), other.yh())};
}

box_t box_t::get_extend(const std::optional<coord_t> &x, const std::optional<coord_t> &y) const {
    coord_t xv = x ? *x : xl();
    coord_t yv = y ? *y : yl();
    return {std::min(xl(), xv), std::min(yl(), yv), std::max(xh(), xv), std::max(yh(), yv)};
}

box_t box_t::get_expand(coord_t dx, coord_t dy) const {
    return {xl() - dx, yl() - dy, xh() + dx, yh() + dy};
}

box_t box_t::get_transform(const transformation &xform) const {
    box_t ans(*this);
    bp::transform(ans, xform);
    return ans;
}

box_t box_t::get_move_by(offset_t dx, offset_t dy) const {
    return {xl() + dx, yl() + dy, xh() + dx, yh() + dy};
}

box_t box_t::get_flip_xy() const { return {intvs[1], intvs[0]}; }

box_t box_t::get_with_interval(bp::orientation_2d_enum orient, interval_type intv) const {
    box_t ans(*this);
    ans.set_interval(orient, std::move(intv));
    return ans;
}

void box_t::set(coord_t xl, coord_t yl, coord_t xh, coord_t yh) {
    intvs[0][0] = xl;
    intvs[0][1] = xh;
    intvs[1][0] = yl;
    intvs[1][1] = yh;
}

void box_t::set_interval(bp::orientation_2d_enum orient, interval_type interval) {
    intvs[static_cast<unsigned int>(orient)] = std::move(interval);
}

box_t &box_t::transform(const transformation &xform) { return bp::transform(*this, xform); }
box_t &box_t::merge(const box_t &other) {
    bp::encompass(*this, other);
    return *this;
}

bool box_t::operator==(const box_t &other) const {
    return intvs[0] == other.intvs[0] && intvs[1] == other.intvs[1];
}

} // namespace cbag
