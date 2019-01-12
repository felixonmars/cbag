#ifndef CBAG_LAYOUT_TECH_UTIL_H
#define CBAG_LAYOUT_TECH_UTIL_H

#include <string>

#include <cbag/common/layer_t.h>
#include <cbag/layout/tech.h>

namespace cbag {
namespace layout {

lay_t layer_id_at(const tech &t, const std::string &layer);

purp_t purpose_id_at(const tech &t, const std::string &purpose);

layer_t layer_t_at(const tech &t, const std::string &layer, const std::string &purpose);

layer_t layer_t_at(const lp_lookup &lp, const std::string &layer, const std::string &purpose);

std::string get_pin_purpose_name(const tech &t);

std::string get_default_purpose_name(const tech &t);

const std::string &get_via_id(const tech &t, layer_t bot_layer, layer_t top_layer);

offset_t get_min_length(const tech &t, const std::string &layer, const std::string &purpose,
                        offset_t width, bool even);

em_specs_t get_metal_em_specs(const tech &t, layer_t key, offset_t width, offset_t length,
                              bool vertical, int_t dc_temp, int_t rms_dt);

} // namespace layout
} // namespace cbag

#endif
