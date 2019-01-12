#include <algorithm>

#include <cbag/layout/tech_util.h>
#include <cbag/layout/wire_info.h>

namespace cbag {
namespace layout {

wire_info::wire_info(std::vector<std::tuple<int_t, offset_t>> &&widths)
    : widths_(std::move(widths)) {}

offset_t wire_info::get_min_length(const tech &t, int_t level, bool even) const {
    offset_t ans = 0;
    for (const auto &key : t.get_lay_purp_list(level)) {
        for (const auto &[htr, w] : widths_) {
            ans = std::max(ans, t.get_min_length(key, w, even));
        }
    }
    return ans;
}

offset_t wire_info::get_min_space(const tech &t, int_t level, space_type sp_type, bool even) const {
    const auto &key = t.get_lay_purp_list(level)[0];
    return t.get_min_space(key, std::get<1>(widths_[0]), sp_type, even);
}

offset_t wire_info::get_edge_wire_width() const { return std::get<1>(widths_[0]); }

offset_t wire_info::get_total_width(offset_t pitch2) const {
    auto &[htr, w] = widths_[0];
    return w + pitch2 * (std::get<0>(widths_[widths_.size() - 1]) - htr);
}

em_specs_t wire_info::get_metal_em_specs(const tech &t, layer_t key, offset_t length, bool vertical,
                                         int_t dc_temp, int_t rms_dt) const {
    double idc = 0;
    double iac_rms = 0;
    double iac_peak = 0;
    for (const auto &[htr, w] : widths_) {
        auto [idc_cur, iac_rms_cur, iac_peak_cur] =
            cbag::layout::get_metal_em_specs(t, key, w, length, vertical, dc_temp, rms_dt);
        idc += idc_cur;
        iac_rms += iac_rms_cur;
        iac_peak += iac_peak_cur;
    }

    return {idc, iac_rms, iac_peak};
}

} // namespace layout
} // namespace cbag
