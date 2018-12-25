
#include <cbag/common/box_t.h>

namespace cbag {

box_t::box_t() noexcept : intvs{{0, 0}, {0, 0}} {}

box_t::box_t(coord_t xl, coord_t yl, coord_t xh, coord_t yh) noexcept : intvs{{xl, xh}, {yl, yh}} {}

box_t::box_t(orient_2d orient, coord_t tl, coord_t th, coord_t pl, coord_t ph) noexcept {
    auto orient_code = static_cast<orient_2d_t>(orient);
    intvs[orient_code][0] = tl;
    intvs[orient_code][1] = th;
    intvs[1 - orient_code][0] = pl;
    intvs[1 - orient_code][1] = ph;
}

box_t box_t::get_invalid_box() noexcept { return {0, 0, -1, -1}; }

} // namespace cbag
