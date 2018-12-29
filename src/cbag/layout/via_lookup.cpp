#include <fmt/core.h>

#include <yaml-cpp/yaml.h>

#include "yaml-cpp/tuple.h"
#include "yaml-cpp/unordered_map.h"

#include <cbag/layout/via_lookup.h>

namespace cbag {
namespace layout {

lay_t get_layer_id(const lay_map_t &lay_map, const std::string &layer) {
    auto iter = lay_map.find(layer);
    if (iter == lay_map.end())
        throw std::out_of_range("Cannot find layer ID for layer: " + layer);
    return iter->second;
}

purp_t get_purpose_id(const purp_map_t &purp_map, const std::string &purpose) {
    auto iter = purp_map.find(purpose);
    if (iter == purp_map.end())
        throw std::out_of_range("Cannot find purpose ID for purpose: " + purpose);
    return iter->second;
}

vlayers_t parse_via_layers(const YAML::Node &node, const lay_map_t &lay_map,
                           const purp_map_t &purp_map) {
    return {layer_t{get_layer_id(lay_map, node[0][0].as<std::string>()),
                    get_purpose_id(purp_map, node[0][1].as<std::string>())},
            layer_t{get_layer_id(lay_map, node[1][0].as<std::string>()),
                    get_purpose_id(purp_map, node[1][1].as<std::string>())}};
}

via_lookup::via_lookup() = default;

via_lookup::via_lookup(const YAML::Node &parent, const lay_map_t &lay_map,
                       const purp_map_t &purp_map)
    : name_map(parent["via_name"].as<vname_map_t>()), lp_map(parent["via_layers"].as<vlp_map_t>()) {
    for (const auto &node : parent["via_id"]) {
        id_map.emplace(parse_via_layers(node.first, lay_map, purp_map),
                       node.second.as<std::string>());
    }
}

via_lay_purp_t via_lookup::get_via_layer_purpose(const std::string &key) const {
    auto iter = lp_map.find(key);
    if (iter == lp_map.end()) {
        throw std::out_of_range(fmt::format("Cannot find via ID: {}", key));
    }
    return iter->second;
}

} // namespace layout
} // namespace cbag
