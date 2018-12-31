#include <algorithm>

#include <fmt/core.h>
#include <yaml-cpp/yaml.h>

#include "yaml-cpp/unordered_map.h"

#include <cbag/layout/tech.h>
#include <cbag/yaml/common.h>

namespace cbag {
namespace layout {

std::vector<std::pair<offset_t, offset_t>> make_w_sp_vec(const YAML::Node &node) {
    std::vector<std::pair<offset_t, offset_t>> ans;
    ans.reserve(node.size());
    for (const auto &val : node) {
        ans.emplace_back(cbagyaml::get_int<offset_t>(val[0]), cbagyaml::get_int<offset_t>(val[1]));
    }
    return ans;
}

sp_map_t make_space_map(const YAML::Node &node) {
    sp_map_t ans;
    for (const auto &pair : node) {
        ans.emplace(pair.first.as<std::string>(), make_w_sp_vec(pair.second));
    }
    return ans;
}

tech::tech(const std::string &tech_fname) {
    YAML::Node node = YAML::LoadFile(tech_fname);

    lay_map = node["layer"].as<lay_map_t>();
    purp_map = node["purpose"].as<purp_map_t>();
    vlookup = via_lookup(node, lay_map, purp_map);
    make_pin_obj = node["make_pin_obj"].as<bool>();

    std::string pin_purp = node["pin_purpose"].as<std::string>();
    std::string def_purp = node["default_purpose"].as<std::string>();
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

    // populate layer type map
    for (const auto &pair : node["layer_type"]) {
        std::string lay_name = pair.first.as<std::string>();
        auto lay_iter = lay_map.find(lay_name);
        if (lay_iter == lay_map.end()) {
            throw std::out_of_range(
                fmt::format("Cannot find layer ID for layer {} in type map", lay_name));

        } else {
            lay_t lay_id = lay_iter->second;
            lay_type_map[lay_id] = pair.second.as<std::string>();
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
} // namespace layout

purp_t tech::get_pin_purpose() const { return pin_purpose; }

purp_t tech::get_default_purpose() const { return default_purpose; }

bool tech::get_make_pin() const { return make_pin_obj; }

std::string tech::get_purpose_name(purp_t purp_id) const {
    auto iter = std::find_if(
        purp_map.begin(), purp_map.end(),
        [&purp_id](const std::pair<std::string, purp_t> &v) { return purp_id == v.second; });
    return iter->first;
}

std::optional<lay_t> tech::get_layer_id(const std::string &layer) const {
    auto iter = lay_map.find(layer);
    if (iter == lay_map.end())
        return {};
    return iter->second;
}

std::optional<purp_t> tech::get_purpose_id(const std::string &purpose) const {
    if (purpose.empty()) {
        return default_purpose;
    }
    auto iter = purp_map.find(purpose);
    if (iter == purp_map.end())
        return {};
    return iter->second;
}

std::string tech::get_layer_type(lay_t lay_id) const {
    auto iter = lay_type_map.find(lay_id);
    return (iter == lay_type_map.end()) ? "" : iter->second;
}

offset_t tech::get_min_space(const std::string &layer_type, offset_t width,
                             space_type sp_type) const {
    auto map_iter = sp_map_grp.find((sp_type == space_type::SAME_COLOR) ? sp_sc_type : sp_type);
    if (map_iter == sp_map_grp.end())
        throw std::out_of_range(
            fmt::format("Cannot find space type {}", static_cast<enum_t>(sp_type)));

    const auto &cur_map = map_iter->second;
    auto vec_iter = cur_map.find(layer_type);
    if (vec_iter == cur_map.end())
        throw std::out_of_range(fmt::format("Cannot find layer type {}", layer_type));

    const auto &w_sp_list = vec_iter->second;

    for (const auto &[w_spec, sp] : w_sp_list) {
        if (width <= w_spec)
            return sp;
    }
    return w_sp_list[w_sp_list.size() - 1].second;
}

via_lay_purp_t tech::get_via_layer_purpose(const std::string &key) const {
    return vlookup.get_via_layer_purpose(key);
}

} // namespace layout
} // namespace cbag
