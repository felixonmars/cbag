#include <cbag/common/box_t_util.h>
#include <cbag/enum/space_type.h>
#include <cbag/layout/polygon.h>
#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech.h>
#include <cbag/layout/track_info.h>
#include <cbag/layout/via_param_util.h>
#include <cbag/layout/wire_info.h>

namespace cbag {
namespace layout {

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const box_t &obj) {
    auto tech_ptr = grid.get_tech();
    auto level = tech_ptr->get_level(key);
    std::array<offset_t, 2> ans{0, 0};
    if (level) {
        auto dir = grid.get_track_info(*level).get_direction();
        auto pdir = perpendicular(dir);
        auto width = get_dim(obj, pdir);
        ans[to_int(dir)] = tech_ptr->get_min_space(key, width, space_type::LINE_END, false);
        ans[to_int(pdir)] = tech_ptr->get_min_space(key, width, space_type::DIFF_COLOR, false);
    }
    return ans;
}

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon90 &obj) {
    // TODO: add margins around polygon?
    return {0, 0};
}

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon45 &obj) {
    // TODO: add margins around polygon?
    return {0, 0};
}

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon &obj) {
    // TODO: add margins around polygon?
    return {0, 0};
}

std::array<offset_t, 2> get_via_extensions(const routing_grid &grid, direction vdir, int level,
                                           cnt_t ntr, cnt_t adj_ntr) {
    auto adj_level = get_adj_level(vdir, level);
    auto tr_info = grid.get_track_info(level);
    auto adj_tr_info = grid.get_track_info(adj_level);
    auto dir = tr_info.get_direction();
    auto adj_dir = adj_tr_info.get_direction();
    if (dir == adj_dir) {
        // TODO: implement for layers with same direction
        throw std::invalid_argument(
            "get_via_extensions() right now requires two layers with different directions.");
    }
    auto tech = *grid.get_tech();
    auto wire_info = tr_info.get_wire_info(ntr);
    auto adj_wire_info = adj_tr_info.get_wire_info(adj_ntr);
    auto key = tech.get_lay_purp_list(level)[0];
    auto adj_key = tech.get_lay_purp_list(adj_level)[0];
    auto via_id = tech.get_via_id(vdir, key, adj_key);
    vector vbox_dim;
    vbox_dim[to_int(dir)] = wire_info.get_edge_wire_width();
    vbox_dim[to_int(adj_dir)] = adj_wire_info.get_edge_wire_width();
    auto via_param = tech.get_via_param(vbox_dim, via_id, vdir, dir, adj_dir, true);

    if (empty(via_param)) {
        auto vidx = to_int(vdir);
        std::array<int, 2> lev_arr;
        lev_arr[vidx] = level;
        lev_arr[1 - vidx] = adj_level;
        std::array<cnt_t, 2> ntr_arr;
        ntr_arr[vidx] = ntr;
        ntr_arr[1 - vidx] = adj_ntr;
        throw std::invalid_argument(
            fmt::format("Cannot draw via with bot_level={}, bot_ntr={}, top_ntr={}", lev_arr[0],
                        ntr_arr[0], ntr_arr[1]));
    }

    return get_via_extensions(via_param, vbox_dim, vdir, dir, adj_dir);
}

offset_t get_line_end_space_htr(const routing_grid &grid, direction vdir, int level, cnt_t ntr) {
    auto sp_level = get_adj_level(vdir, level);
    auto tr_info = grid.get_track_info(level);
    if (tr_info.get_direction() == grid.get_track_info(sp_level).get_direction())
        throw std::invalid_argument("space layer must be orthogonal to wire layer.");

    auto via_ext = get_via_extensions(grid, vdir, level, ntr, 1)[to_int(vdir)];
    auto winfo = tr_info.get_wire_info(ntr);
    auto sp_le = winfo.get_min_space(*grid.get_tech(), level, space_type::LINE_END, false);
    return tr_info.space_to_htr(2 * via_ext + sp_le);
}

} // namespace layout
} // namespace cbag
