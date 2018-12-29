#include <cbag/layout/via_info.h>

namespace cbag {
namespace layout {

via_info::via_info() = default;

via_info::via_info(std::string cut_type, dist_t cut_w, dist_t cut_h, double_t scale)
    : cut_type(std::move(cut_type)), cut_dim{cut_w, cut_h}, via_scale(scale) {}

} // namespace layout
} // namespace cbag
