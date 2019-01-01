#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation.h>
#include <cbag/common/vector.h>
#include <cbag/layout/via_util.h>

namespace cbag {
namespace layout {

box_t get_via_box(const transformation &xform, const via_param &params, const vector &offset,
                  const vector &enc) {
    auto nx = params.num[0];
    auto ny = params.num[1];
    auto via_w = nx * params.cut_dim[0] + (nx - 1) * params.cut_spacing[0];
    auto via_h = ny * params.cut_dim[1] + (ny - 1) * params.cut_spacing[1];

    auto xl = static_cast<coord_t>(offset[0] - (via_w / 2) - enc[0]);
    auto yl = static_cast<coord_t>(offset[1] - (via_h / 2) - enc[1]);
    auto xh = static_cast<coord_t>(xl + via_w + enc[0]);
    auto yh = static_cast<coord_t>(yl + via_h + enc[1]);

    box_t r{xl, yl, xh, yh};
    transform(r, xform);
    return r;
}

box_t get_bot_box(const via &v) {
    auto &param = v.get_params();
    return get_via_box(v.xform, param, param.lay1_off, param.lay1_enc);
}

box_t get_top_box(const via &v) {
    auto &param = v.get_params();
    return get_via_box(v.xform, param, param.lay2_off, param.lay2_enc);
}

} // namespace layout
} // namespace cbag
