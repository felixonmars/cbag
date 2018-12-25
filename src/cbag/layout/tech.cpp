#include <algorithm>

#include <fmt/core.h>
#include <yaml-cpp/yaml.h>

#include "yaml-cpp/unordered_map.h"

#include <cbag/layout/tech.h>

namespace cbag {
namespace layout {

std::vector<offset_t> make_sp_vec(const YAML::Node &node) {
    std::vector<offset_t> ans;
    ans.reserve(node.size());
    for (const auto &val : node) {
        if (val.as<double>() == std::numeric_limits<double>::infinity()) {
            ans.emplace_back(std::numeric_limits<offset_t>::max());
        } else {
            ans.emplace_back(val.as<offset_t>());
        }
    }
    return ans;
}

sp_map_t make_space_map(const YAML::Node &node) {
    sp_map_t ans;
    for (const auto &pair : node) {
        ans.emplace(pair.first.as<std::string>(),
                    std::make_pair(make_sp_vec(pair.second["w_list"]),
                                   make_sp_vec(pair.second["sp_list"])));
    }
    return ans;
}

tech::tech(const std::string &tech_fname) {
    YAML::Node node = YAML::LoadFile(tech_fname);

    lay_map = node["layer"].as<lay_map_t>();
    purp_map = node["purpose"].as<purp_map_t>();
    via_map = node["via_layers"].as<via_map_t>();
    make_pin_obj = node["make_pin_obj"].as<bool>();

    std::string pin_purp = node["pin_purpose"].as<std::string>();
    std::string def_purp = node["default_purpose"].as<std::string>();
    try {
        default_purpose = purp_map.at(def_purp);
    } catch (const std::out_of_range &) {
        throw std::out_of_range(fmt::format("Cannot find default purpose: {}", def_purp));
    }
    try {
        pin_purpose = purp_map.at(pin_purp);
    } catch (const std::out_of_range &) {
        throw std::out_of_range(fmt::format("Cannot find pin purpose: {}", pin_purp));
    }

    // populate layer type map
    for (const auto &pair : node["layer_type"]) {
        std::string lay_name = pair.first.as<std::string>();
        try {
            lay_t lay_id = lay_map.at(lay_name);
            lay_type_map[lay_id] = pair.second.as<std::string>();
        } catch (const std::out_of_range &) {
            throw std::out_of_range(
                fmt::format("Cannot find layer ID for layer {} in type map", lay_name));
        }
    }

    // populate space map
    sp_map_t sp_map;
    sp_map_grp.emplace(space_type::DIFF_COLOR, make_space_map(node["sp_min"]));
    sp_map_grp.emplace(space_type::LINE_END, make_space_map(node["sp_le_min"]));

    auto sp_sc_node = node["sp_sc_min"];
    if (sp_sc_node.IsDefined()) {
        sp_map_grp.emplace(space_type::SAME_COLOR, make_space_map(sp_sc_node));
        sp_sc_type = space_type::SAME_COLOR;
    } else {
        sp_sc_type = space_type::DIFF_COLOR;
    }
}

std::string tech::get_pin_purpose_name() const { return get_purpose_name(pin_purpose); }

std::string tech::get_default_purpose_name() const { return get_purpose_name(default_purpose); }

std::string tech::get_purpose_name(purp_t purp_id) const {
    auto iter = std::find_if(
        purp_map.begin(), purp_map.end(),
        [&purp_id](const std::pair<std::string, purp_t> &v) { return purp_id == v.second; });
    return iter->first;
}

lay_t tech::get_layer_id(const std::string &layer) const {
    try {
        return lay_map.at(layer);
    } catch (const std::out_of_range &) {
        throw std::out_of_range(fmt::format("Cannot find layer: {}", layer));
    }
}

purp_t tech::get_purpose_id(const std::string &purpose) const {
    if (purpose.empty()) {
        return default_purpose;
    }
    try {
        return purp_map.at(purpose);
    } catch (const std::out_of_range &) {
        throw std::out_of_range(fmt::format("Cannot find purpose: {}", purpose));
    }
}

std::string tech::get_layer_type(lay_t lay_id) const {
    auto iter = lay_type_map.find(lay_id);
    return (iter == lay_type_map.end()) ? "" : iter->second;
}

void tech::get_via_layers(const std::string &key, lay_t &bot, lay_t &top) const {
    try {
        auto temp = via_map.at(key);
        bot = temp.first;
        top = temp.second;
    } catch (const std::out_of_range &) {
        throw std::out_of_range(fmt::format("Cannot find via ID: {}", key));
    }
}

offset_t tech::get_min_space(const std::string &layer_type, offset_t width,
                             space_type sp_type) const {
    try {
        const sp_map_t &cur_map =
            sp_map_grp.at((sp_type == space_type::SAME_COLOR) ? sp_sc_type : sp_type);
        const auto &pair = cur_map.at(layer_type);
        std::size_t n = std::min(pair.first.size(), pair.second.size());
        for (std::size_t idx = 0; idx < n; ++idx) {
            if (width <= pair.first[idx])
                return pair.second[idx];
        }
        return pair.second[pair.second.size() - 1];
    } catch (const std::out_of_range &) {
        throw std::out_of_range(fmt::format("Cannot find layer type {} or space type {}",
                                            layer_type, static_cast<enum_t>(sp_type)));
    }
}

offset_t tech::get_min_space(const std::string &layer_type, offset_t width, enum_t sp_type) const {
    return get_min_space(layer_type, width, static_cast<space_type>(sp_type));
}

} // namespace layout
} // namespace cbag
