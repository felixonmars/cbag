#ifndef CBAG_LAYOUT_VIA_UTIL_H
#define CBAG_LAYOUT_VIA_UTIL_H

#include <type_traits>

#include <cbag/layout/via.h>
#include <cbag/common/box_t_util.h>


namespace cbag {

namespace layout {

box_t get_bot_box(const via &v);

box_t get_top_box(const via &v);

template <typename It> void get_via_cuts(const via &v, It out_iter) {
    auto &params = v.get_params();
    auto &[nx, ny] = params.num;
    auto &[vw, vh] = params.cut_dim;
    auto &[spx, spy] = params.cut_spacing;

    auto xoff = (nx * (vw + spx) - spx) / 2;
    auto yoff = (ny * (vh + spy) - spy) / 2;

    offset_t dx = xoff;
    box_t cut_box{0, 0, static_cast<coord_t>(vw), static_cast<coord_t>(vh)};
    for (std::remove_const_t<decltype(nx)> xidx = 0; xidx != nx; ++xidx, dx += spx) {
        offset_t dy = yoff;
        for (std::remove_const_t<decltype(nx)> yidx = 0; yidx != ny; ++yidx, dy += spy) {
            *out_iter = get_move_by(get_transform(cut_box, v.xform), dx, dy);
            ++out_iter;
        }
    }
}

} // namespace layout
} // namespace cbag

#endif
