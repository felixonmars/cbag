#include <cbag/layout/tech_util.h>

namespace cbag {
namespace layout {

layer_t get_layer_t(const tech &t, const std::string &layer, const std::string &purpose) {
    return {t.get_layer_id(layer), t.get_purpose_id(purpose)};
}

std::string get_pin_purpose_name(const tech &t) { return t.get_purpose_name(t.get_pin_purpose()); }

std::string get_default_purpose_name(const tech &t) {
    return t.get_purpose_name(t.get_default_purpose());
}

offset_t get_min_space(const tech &t, const std::string &layer_type, offset_t width,
                       enum_t sp_type) {
    return t.get_min_space(layer_type, width, static_cast<space_type>(sp_type));
}

} // namespace layout
} // namespace cbag
