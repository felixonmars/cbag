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

offset_t get_min_space(const tech &t, const std::string &layer, const std::string &purpose,
                       offset_t width, enum_t sp_type);

offset_t get_min_length(const tech &t, const std::string &layer, const std::string &purpose,
                        offset_t width, bool even);

} // namespace layout
} // namespace cbag

#endif
