#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation.h>
#include <cbag/common/vector.h>
#include <cbag/layout/via_util.h>

namespace cbag {
namespace layout {

box_t get_via_box(const transformation &xform, const via_param &params, const vector &offset,
                  const vector &enc) {
    auto via_w =
        params.num[0] * (params.cut_dim[0] + params.cut_spacing[0]) - params.cut_spacing[0];
    auto via_h =
        params.num[1] * (params.cut_dim[1] + params.cut_spacing[1]) - params.cut_spacing[1];

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
    return get_via_box(v.xform, param, param.off[0], param.enc[0]);
}

box_t get_top_box(const via &v) {
    auto &param = v.get_params();
    return get_via_box(v.xform, param, param.off[1], param.enc[1]);
}

} // namespace layout
} // namespace cbag
