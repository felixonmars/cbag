#include <cbag/common/box_t.h>
#include <cbag/common/transformation_util.h>
#include <cbag/layout/via_param_util.h>

namespace cbag {
namespace layout {

bool empty(const via_param &p) { return p.num[0] == 0; }

offset_t get_arr_dim(const via_param &p, orient_2d orient) {
    auto dir_idx = to_int(orient);
    return p.num[dir_idx] * (p.cut_dim[dir_idx] + p.cut_spacing[dir_idx]) - p.cut_spacing[dir_idx];
}

offset_t get_metal_dim(const via_param &p, orient_2d orient, direction vdir) {
    return get_arr_dim(p, orient) + 2 * p.enc[to_int(vdir)][orient];
}

box_t get_box(const via_param &p, const transformation &xform, direction vdir) {
    auto w2 = get_metal_dim(p, orient_2d::HORIZONTAL, vdir) / 2;
    auto h2 = get_metal_dim(p, orient_2d::VERTICAL, vdir) / 2;
    auto [x, y] = location(xform);
    return {x - w2, y - h2, x + w2, y + h2};
}

std::array<offset_t, 2> get_via_extensions(const via_param &p, vector dim, direction vdir,
                                           orient_2d ex_dir, orient_2d adj_ex_dir) {

    return std::array<offset_t, 2>{
        get_metal_dim(p, ex_dir, vdir) - dim[to_int(ex_dir)],
        get_metal_dim(p, adj_ex_dir, flip(vdir)) - dim[to_int(adj_ex_dir)],
    };
}

} // namespace layout
} // namespace cbag
