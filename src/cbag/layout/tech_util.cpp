#include <fmt/core.h>

#include <cbag/layout/tech_util.h>

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

layer_t layer_t_at(const tech &t, const std::string &layer, const std::string &purpose) {
    return {layer_id_at(t, layer), purpose_id_at(t, purpose)};
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
