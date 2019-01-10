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

std::array<offset_t, 2> get_via_extensions(const routing_grid &grid, int bot_level, cnt_t bot_ntr,
                                           cnt_t top_ntr) {
    auto bot_tr_info = grid.get_track_info(bot_level);
    auto top_tr_info = grid.get_track_info(bot_level + 1);
    auto bot_dir = bot_tr_info.get_direction();
    auto top_dir = top_tr_info.get_direction();
    if (top_dir == bot_dir) {
        // TODO: implement for layers with same direction
        throw std::invalid_argument(
            "get_via_extensions() right now requires two layers with different directions.");
    }
    auto tech = *grid.get_tech();
    auto bot_wire_info = bot_tr_info.get_wire_info(bot_ntr);
    auto top_wire_info = top_tr_info.get_wire_info(top_ntr);
    auto bot_key = tech.get_lay_purp_list(bot_level)[0];
    auto top_key = tech.get_lay_purp_list(bot_level + 1)[0];
    auto via_id = tech.get_via_id(bot_key, top_key);
    vector vbox_dim;
    vbox_dim[to_int(bot_dir)] = bot_wire_info.get_edge_wire_width();
    vbox_dim[to_int(top_dir)] = top_wire_info.get_edge_wire_width();
    auto via_param = tech.get_via_param(vbox_dim, via_id, bot_dir, top_dir, true);

    if (empty(via_param))
        throw std::invalid_argument(
            fmt::format("Cannot draw via with bot_level={}, bot_ntr={}, top_ntr={}", bot_level,
                        bot_ntr, top_ntr));

    return get_via_extensions(via_param, vbox_dim, bot_dir, top_dir);
}

} // namespace layout
} // namespace cbag
