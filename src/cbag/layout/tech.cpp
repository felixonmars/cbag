#include <fmt/format.h>

#include <cbag/layout/tech.h>

namespace cbag {
namespace layout {

tech::tech(lay_map_t lay_map_tmp, purp_map_t purp_map_tmp, via_map_t via_map_tmp,
           const std::string &def_purpose, const std::string &pin_purpose_name, bool make_pin_obj)
    : lay_map(std::move(lay_map_tmp)), purp_map(std::move(purp_map_tmp)),
      via_map(std::move(via_map_tmp)), make_pin_obj(make_pin_obj) {
    auto def_iter = purp_map.find(def_purpose);
    auto end = purp_map.end();
    if (def_iter == end) {
        throw std::invalid_argument(
            fmt::format("Cannot find default purpose {} in purpose map.", def_purpose));
    }
    auto pin_iter = purp_map.find(pin_purpose_name);
    if (pin_iter == end) {
        throw std::invalid_argument(
            fmt::format("Cannot find pin purpose {} in purpose map.", pin_purpose_name));
    }

    default_purpose = def_iter->second;
    pin_purpose = pin_iter->second;
}

lay_t tech::get_layer_id(const char *layer) const {
    std::string key(layer);
    try {
        return lay_map.at(layer);
    } catch (std::out_of_range) {
        throw std::out_of_range(fmt::format("Cannot find layer: {}", key));
    }
}

purp_t tech::get_purpose_id(const char *purpose) const {
    if (purpose == nullptr) {
        return default_purpose;
    }
    std::string key(purpose);
    try {
        return purp_map.at(key);
    } catch (std::out_of_range) {
        throw std::out_of_range(fmt::format("Cannot find purpose: {}", key));
    }
}

void tech::get_via_layers(const std::string &key, lay_t &bot, lay_t &top) const {
    try {
        auto temp = via_map.at(key);
        bot = temp.first;
        top = temp.second;
    } catch (std::out_of_range) {
        throw std::out_of_range(fmt::format("Cannot find via ID: {}", key));
    }
}

} // namespace layout
} // namespace cbag
