#include <cbag/layout/via_param_util.h>

namespace cbag {
namespace layout {

offset_t get_arr_dim(const via_param &p, orient_2d orient) {
    auto dir_idx = to_int(orient);
    return p.num[dir_idx] * (p.cut_dim[dir_idx] + p.cut_spacing[dir_idx]) - p.cut_spacing[dir_idx];
}

offset_t get_metal_dim(const via_param &p, orient_2d orient, cnt_t level) {
    return get_arr_dim(p, orient) + 2 * p.enc[level][orient];
}

} // namespace layout
} // namespace cbag
