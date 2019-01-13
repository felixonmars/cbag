#ifndef CBAG_LAYOUT_TECH_UTIL_H
#define CBAG_LAYOUT_TECH_UTIL_H

#include <string>

#include <cbag/common/layer_t.h>
#include <cbag/layout/tech.h>

namespace cbag {
namespace layout {

class wire_width;

lay_t layer_id_at(const tech &t, const std::string &layer);

purp_t purpose_id_at(const tech &t, const std::string &purpose);

layer_t layer_t_at(const tech &t, const std::string &layer, const std::string &purpose);

layer_t layer_t_at(const lp_lookup &lp, const std::string &layer, const std::string &purpose);

layer_t get_test_lay_purp(const tech &t, int_t level);

const std::string &get_pin_purpose_name(const tech &t);

const std::string &get_default_purpose_name(const tech &t);

offset_t get_min_length(const tech &t, const std::string &layer, const std::string &purpose,
                        offset_t width, bool even);

offset_t get_min_length(const tech &t, int_t level, const wire_width &wire_w, bool even);

offset_t get_min_space(const tech &t, int_t level, const wire_width &wire_w, space_type sp_type,
                       bool even);

em_specs_t get_metal_em_specs(const tech &t, layer_t key, offset_t width, offset_t length,
                              bool vertical, int_t dc_temp, int_t rms_dt);

em_specs_t get_metal_em_specs(const tech &t, int_t level, const wire_width &wire_w, offset_t length,
                              bool vertical, int_t dc_temp, int_t rms_dt);

em_specs_t get_via_em_specs(const tech &t, direction vdir, layer_t key, layer_t adj_key,
                            std::array<offset_t, 2> cut_dim, std::array<offset_t, 2> m_dim,
                            std::array<offset_t, 2> adj_m_dim, bool array, int_t dc_temp,
                            int_t rms_dt);

} // namespace layout
} // namespace cbag

#endif
