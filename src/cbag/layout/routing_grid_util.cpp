#include <cbag/common/box_t_util.h>
#include <cbag/enum/space_type.h>
#include <cbag/layout/polygon.h>
#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech.h>

namespace cbag {
namespace layout {

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const box_t &obj) {
    auto tech_ptr = grid.get_tech();
    auto level = tech_ptr->get_level(key);
    std::array<offset_t, 2> ans{0, 0};
    if (level) {
        auto dir = grid.get_direction(*level);
        auto pdir = perpendicular(dir);
        auto width = get_dim(obj, pdir);
        ans[to_int(dir)] = tech_ptr->get_min_space(key, width, space_type::LINE_END);
        ans[to_int(pdir)] = tech_ptr->get_min_space(key, width, space_type::DIFF_COLOR);
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

} // namespace layout
} // namespace cbag
