
#include <cbag/common/box_t_util.h>

namespace cbag {

box_t::box_t() noexcept : intvs{interval_t{0, 0}, interval_t{0, 0}} {}

box_t::box_t(coord_t xl, coord_t yl, coord_t xh, coord_t yh) noexcept
    : intvs{interval_t{xl, xh}, interval_t{yl, yh}} {}

box_t::box_t(orient_2d orient, coord_t tl, coord_t th, coord_t pl, coord_t ph) noexcept {
    auto orient_code = to_int(orient);
    intvs[orient_code][0] = tl;
    intvs[orient_code][1] = th;
    intvs[1 - orient_code][0] = pl;
    intvs[1 - orient_code][1] = ph;
}

box_t box_t::get_invalid_box() noexcept { return {0, 0, -1, -1}; }

bool box_t::operator==(const box_t &rhs) const noexcept {
    return intvs[0] == rhs.intvs[0] && intvs[1] == rhs.intvs[1];
}

std::ostream &operator<<(std::ostream &os, const box_t &value) { return os << to_string(value); }

} // namespace cbag
