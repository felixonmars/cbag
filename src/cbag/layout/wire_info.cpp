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
    const auto &key = t.get_lay_purp_list(level)[0];
    return t.get_min_space(key, std::get<1>(widths_[0]), sp_type, even);
}

offset_t wire_info::get_wire_width(offset_t pitch2) const {
    auto &[htr, w] = widths_[0];
    return w + pitch2 * (std::get<0>(widths_.back()) - htr);
}

} // namespace layout
} // namespace cbag
