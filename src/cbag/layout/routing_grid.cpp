#include <fmt/core.h>

#include <cbag/layout/routing_grid.h>

namespace cbag {
namespace layout {

track_info::track_info() = default;

track_info::track_info(orient_2d tr_dir, offset_t tr_w, offset_t tr_sp,
                       const std::vector<std::array<offset_t, 2>> &intv_list)
    : dir(tr_dir), w(tr_w), sp(tr_sp) {
    for (const auto &intv : intv_list) {
        bool success = w_intvs.emplace(false, false, intv);
        if (!success) {
            throw std::invalid_argument(fmt::format(
                "Overlapping routing grid width intervals : [{}, {})", intv[0], intv[1]));
        }
    }
}

routing_grid::routing_grid() = default;

} // namespace layout
} // namespace cbag
