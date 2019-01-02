#include <algorithm>

#include <fmt/core.h>

#include <yaml-cpp/yaml.h>

#include "yaml-cpp/unordered_map.h"

#include <cbag/layout/lp_lookup.h>

namespace cbag {
namespace layout {

lp_lookup::lp_lookup() = default;

lp_lookup::lp_lookup(const YAML::Node &parent)
    : lay_map(parent["layer"].as<lay_map_t>()), purp_map(parent["purpose"].as<purp_map_t>()) {

    auto def_purp = parent["default_purpose"].as<std::string>();
    auto pin_purp = parent["pin_purpose"].as<std::string>();

    auto purp_iter = purp_map.find(def_purp);
    if (purp_iter == purp_map.end()) {
        throw std::out_of_range(fmt::format("Cannot find default purpose: {}", def_purp));
    } else {
        default_purpose = purp_iter->second;
    }
    purp_iter = purp_map.find(pin_purp);
    if (purp_iter == purp_map.end()) {
        throw std::out_of_range(fmt::format("Cannot find pin purpose: {}", pin_purp));
    } else {
        pin_purpose = purp_iter->second;
    }
}

purp_t lp_lookup::get_default_purpose() const { return default_purpose; }

purp_t lp_lookup::get_pin_purpose() const { return pin_purpose; }

std::string lp_lookup::get_purpose_name(purp_t purp_id) const {
    auto iter = std::find_if(
        purp_map.begin(), purp_map.end(),
        [&purp_id](const std::pair<std::string, purp_t> &v) { return purp_id == v.second; });
    return iter->first;
}

std::optional<lay_t> lp_lookup::get_layer_id(const std::string &layer) const {
    auto iter = lay_map.find(layer);
    if (iter == lay_map.end())
        return {};
    return iter->second;
}

std::optional<purp_t> lp_lookup::get_purpose_id(const std::string &purpose) const {
    if (purpose.empty()) {
        return default_purpose;
    }
    auto iter = purp_map.find(purpose);
    if (iter == purp_map.end())
        return {};
    return iter->second;
}

} // namespace layout
} // namespace cbag
