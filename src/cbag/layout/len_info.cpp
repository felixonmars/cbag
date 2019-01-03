#include <algorithm>

#include <cbag/layout/len_info.h>
#include <cbag/util/math.h>

namespace cbag {
namespace layout {

len_info::len_info() = default;

len_info::len_info(std::vector<max_len_data> &&w_al, std::vector<max_len_data> &&md_al)
    : w_al_list(std::move(w_al)), md_al_list(std::move(md_al)) {}

offset_t get_min_len_width(offset_t w, const std::vector<max_len_data> &vec) {
    for (const auto &[width, area, min_len] : vec) {
        if (w <= width) {
            return std::max(min_len, util::ceil(area, w));
        }
    }
    return 0;
}

offset_t get_min_len_max_dim(offset_t w, offset_t len, const std::vector<max_len_data> &vec) {
    auto ans = len;
    for (const auto &[max_dim, area, min_len] : vec) {
        if (std::max(w, ans) > max_dim)
            return ans;
        ans = std::max({ans, min_len, util::ceil(area, w)});
    }
    return ans;
}

offset_t len_info::get_min_length(offset_t w, bool even) const {
    offset_t ans = get_min_len_width(w, w_al_list);
    ans = get_min_len_max_dim(w, ans, md_al_list);

    return (even) ? ans + (ans & 1) : ans;
}

} // namespace layout
} // namespace cbag
