#include <algorithm>

#include <cbag/layout/tech.h>
#include <cbag/layout/wire_info.h>

namespace cbag {
namespace layout {

wire_info::wire_info(std::vector<std::tuple<int, offset_t>> &&widths)
    : widths_(std::move(widths)) {}

offset_t wire_info::get_min_length(const tech &t, int level, bool even) const {
    offset_t ans = 0;
    for (const auto &key : t.get_lay_purp_list(level)) {
        for (const auto &[htr, w] : widths_) {
            ans = std::max(ans, t.get_min_length(key, w, even));
        }
    }
    return ans;
}

offset_t wire_info::get_min_space(const tech &t, int level, space_type sp_type, bool even) const {
    offset_t ans = 0;
    for (const auto &key : t.get_lay_purp_list(level)) {
        for (const auto &[htr, w] : widths_) {
            ans = std::max(ans, t.get_min_space(key, w, sp_type, even));
        }
    }
    return ans;
}

} // namespace layout
} // namespace cbag
