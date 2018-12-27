#ifndef CBAG_LAYOUT_TECH_UTIL_H
#define CBAG_LAYOUT_TECH_UTIL_H

#include <string>

#include <cbag/common/layer_t.h>
#include <cbag/layout/tech.h>

namespace cbag {
namespace layout {

layer_t get_layer_t(const tech &t, const std::string &layer, const std::string &purpose);

std::string get_pin_purpose_name(const tech &t);

std::string get_default_purpose_name(const tech &t);

offset_t get_min_space(const tech &t, const std::string &layer_type, offset_t width,
                       enum_t sp_type);

} // namespace layout
} // namespace cbag

#endif
