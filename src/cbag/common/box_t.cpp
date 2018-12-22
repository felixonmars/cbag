
#include <algorithm>

#include <fmt/core.h>

#include <cbag/common/box_t.h>
#include <cbag/common/transformation.h>
#include <cbag/util/floor_half.h>

namespace cbag {

box_t::box_t() : intvs{{0, 0}, {0, 0}} {}

box_t::box_t(interval_type horizontal, interval_type vertical)
    : intvs{std::move(horizontal), std::move(vertical)} {}

box_t::box_t(coord_t xl, coord_t yl, coord_t xh, coord_t yh) : intvs{{xl, xh}, {yl, yh}} {}

box_t::box_t(uint8_t orient_code, coord_t tl, coord_t th, coord_t pl, coord_t ph) {
    intvs[orient_code][0] = tl;
    intvs[orient_code][1] = th;
    intvs[1 - orient_code][0] = pl;
    intvs[1 - orient_code][1] = ph;
}

coord_t box_t::xl() const { return intvs[0][0]; }
coord_t box_t::yl() const { return intvs[1][0]; }
coord_t box_t::xh() const { return intvs[0][1]; }
coord_t box_t::yh() const { return intvs[1][1]; }
coord_t box_t::xm() const { return cbag::util::floor_half(xl() + xh()); }
coord_t box_t::ym() const { return cbag::util::floor_half(yl() + yh()); }
coord_t box_t::w() const { return xh() - xl(); }
coord_t box_t::h() const { return yh() - yl(); }

offset_t box_t::get_dim(uint8_t orient_code) const {
    return intvs[orient_code][1] - intvs[orient_code][0];
}

coord_t box_t::get_coord(uint8_t orient_code, uint8_t bnd_code) const {
    return get_interval(orient_code)[bnd_code];
}

auto box_t::get_interval(uint8_t orient_code) const -> const interval_type & {
    return intvs[orient_code];
}

std::string box_t::to_string() const {
    return fmt::format("BBox({}, {}, {}, {})", xl(), yl(), xh(), yh());
}

bool box_t::is_physical() const { return w() > 0 && h() > 0; }
bool box_t::is_valid() const { return w() >= 0 && h() >= 0; }
bool box_t::overlaps(const box_t &other) const { return bp::intersects(*this, other, true); }

box_t &box_t::merge(const box_t &other) {
    if (!other.is_valid())
        return *this;
    if (!is_valid())
        *this = other;
    bp::encompass(*this, other);
    return *this;
}

box_t box_t::get_merge(const box_t &other) const { return box_t(*this).merge(other); }

box_t &box_t::intersect(const box_t &other) {
    set(std::max(xl(), other.xl()), std::max(yl(), other.yl()), std::min(xh(), other.xh()),
        std::min(yh(), other.yh()));
    return *this;
}

box_t box_t::get_intersect(const box_t &other) const { return box_t(*this).intersect(other); }

box_t &box_t::extend(const std::optional<coord_t> &x, const std::optional<coord_t> &y) {
    if (is_valid()) {
        coord_t xv = x ? *x : xl();
        coord_t yv = y ? *y : yl();
        set(std::min(xl(), xv), std::min(yl(), yv), std::max(xh(), xv), std::max(yh(), yv));
    }
    return *this;
}

box_t box_t::get_extend(const std::optional<coord_t> &x, const std::optional<coord_t> &y) const {
    return box_t(*this).extend(x, y);
}

box_t &box_t::expand(coord_t dx, coord_t dy) {
    if (is_valid()) {
        set(xl() - dx, yl() - dy, xh() + dx, yh() + dy);
    }
    return *this;
}

box_t box_t::get_expand(coord_t dx, coord_t dy) const { return box_t(*this).expand(dx, dy); }

box_t &box_t::transform(const transformation &xform) { return bp::transform(*this, xform); }

box_t box_t::get_transform(const transformation &xform) const {
    return box_t(*this).transform(xform);
}

box_t &box_t::move_by(offset_t dx, offset_t dy) {
    set(xl() + dx, yl() + dy, xh() + dx, yh() + dy);
    return *this;
}

box_t box_t::get_move_by(offset_t dx, offset_t dy) const { return box_t(*this).move_by(dx, dy); }

box_t &box_t::flip_xy() {
    auto tmp = intvs[0];
    intvs[0] = intvs[1];
    intvs[1] = tmp;
    return *this;
}

box_t box_t::get_flip_xy() const { return box_t(*this).flip_xy(); }

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

bool box_t::operator==(const box_t &other) const {
    return intvs[0] == other.intvs[0] && intvs[1] == other.intvs[1];
}

box_t box_t::invalid_box() { return {0, 0, -1, -1}; }

} // namespace cbag
