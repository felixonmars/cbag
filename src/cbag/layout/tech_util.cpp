#include <fmt/core.h>

#include <cbag/layout/tech_util.h>
#include <cbag/layout/wire_width.h>

namespace cbag {
namespace layout {

lay_t layer_id_at(const tech &t, const std::string &layer) {
    auto ans = t.get_layer_id(layer);
    if (!ans)
        throw std::out_of_range(fmt::format("Cannot find layer: {}", layer));
    return *ans;
}

purp_t purpose_id_at(const tech &t, const std::string &purpose) {
    auto ans = t.get_purpose_id(purpose);
    if (!ans)
        throw std::out_of_range(fmt::format("Cannot find purpose: {}", purpose));
    return *ans;
}

lay_t layer_id_at(const lp_lookup &lp, const std::string &layer) {
    auto ans = lp.get_layer_id(layer);
    if (!ans)
        throw std::out_of_range(fmt::format("Cannot find layer: {}", layer));
    return *ans;
}

purp_t purpose_id_at(const lp_lookup &lp, const std::string &purpose) {
    auto ans = lp.get_purpose_id(purpose);
    if (!ans)
        throw std::out_of_range(fmt::format("Cannot find purpose: {}", purpose));
    return *ans;
}

layer_t layer_t_at(const tech &t, const std::string &layer, const std::string &purpose) {
    return {layer_id_at(t, layer), purpose_id_at(t, purpose)};
}

layer_t layer_t_at(const lp_lookup &lp, const std::string &layer, const std::string &purpose) {
    return {layer_id_at(lp, layer), purpose_id_at(lp, purpose)};
}

const std::string &get_via_id(const tech &t, layer_t bot_layer, layer_t top_layer) {
    return t.get_via_id(direction::LOWER, bot_layer, top_layer);
}

std::string get_pin_purpose_name(const tech &t) { return t.get_purpose_name(t.get_pin_purpose()); }

std::string get_default_purpose_name(const tech &t) {
    return t.get_purpose_name(t.get_default_purpose());
}

offset_t get_min_length(const tech &t, const std::string &layer, const std::string &purpose,
                        offset_t width, bool even) {
    return t.get_min_length(layer_t_at(t, layer, purpose), width, even);
}

em_specs_t get_metal_em_specs(const tech &t, layer_t key, offset_t width, offset_t length,
                              bool vertical, int_t dc_temp, int_t rms_dt) {
    auto &layer = t.get_layer_name(key.first);
    auto &purpose = t.get_purpose_name(key.second);
    return t.get_metal_em_specs(layer, purpose, width, length, vertical, dc_temp, rms_dt);
}

em_specs_t get_via_em_specs(const tech &t, direction vdir, layer_t key, layer_t adj_key,
                            std::array<offset_t, 2> cut_dim, std::array<offset_t, 2> m_dim,
                            std::array<offset_t, 2> adj_m_dim, bool array, int_t dc_temp,
                            int_t rms_dt) {
    auto &layer = t.get_layer_name(key.first);
    auto &purpose = t.get_purpose_name(key.second);
    auto &adj_layer = t.get_layer_name(adj_key.first);
    auto &adj_purpose = t.get_purpose_name(adj_key.second);
    return t.get_via_em_specs(to_int(vdir), layer, purpose, adj_layer, adj_purpose, cut_dim[0],
                              cut_dim[1], m_dim[0], m_dim[1], adj_m_dim[0], adj_m_dim[1], array,
                              dc_temp, rms_dt);
}

offset_t get_min_length(const tech &t, int_t level, const wire_width &wire_w, bool even) {
    offset_t ans = 0;
    auto key = t.get_lay_purp_list(level)[0];
    auto wend = wire_w.end_width();
    for (auto witer = wire_w.begin_width(); witer != wend; ++witer) {
        ans = std::max(ans, t.get_min_length(key, *witer, even));
    }
    return ans;
}

offset_t get_min_space(const tech &t, int_t level, const wire_width &wire_w, space_type sp_type,
                       bool even) {
    const auto &key = t.get_lay_purp_list(level)[0];
    return t.get_min_space(key, wire_w.get_edge_wire_width(), sp_type, even);
}

em_specs_t get_metal_em_specs(const tech &t, int_t level, const wire_width &wire_w, offset_t length,
                              bool vertical, int_t dc_temp, int_t rms_dt) {
    auto key = t.get_lay_purp_list(level)[0];

    double idc = 0;
    double iac_rms = 0;
    double iac_peak = 0;
    auto wend = wire_w.end_width();
    for (auto witer = wire_w.begin_width(); witer != wend; ++witer) {
        auto [idc_cur, iac_rms_cur, iac_peak_cur] =
            cbag::layout::get_metal_em_specs(t, key, *witer, length, vertical, dc_temp, rms_dt);
        idc += idc_cur;
        iac_rms += iac_rms_cur;
        iac_peak += iac_peak_cur;
    }

    return {idc, iac_rms, iac_peak};
}

} // namespace layout
} // namespace cbag
