#include <algorithm>

#include <fmt/core.h>
#include <yaml-cpp/yaml.h>

#include "yaml-cpp/unordered_map.h"

#include <cbag/layout/tech.h>
#include <cbag/layout/tech_util.h>
#include <cbag/yaml/common.h>
#include <cbag/yaml/len_info.h>

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

sp_map_t make_space_map(const YAML::Node &node, const lp_lookup &lp_map) {
    sp_map_t ans;
    for (const auto &pair : node) {
        auto [lay_name, purp_name] = pair.first.as<std::pair<std::string, std::string>>();
        ans.emplace(layer_t_at(lp_map, lay_name, purp_name), make_w_sp_vec(pair.second));
    }
    return ans;
}

len_map_t make_len_map(const YAML::Node &node, const lp_lookup &lp_map) {
    len_map_t ans;
    for (const auto &pair : node) {
        auto [lay_name, purp_name] = pair.first.as<std::pair<std::string, std::string>>();
        ans.emplace(layer_t_at(lp_map, lay_name, purp_name), pair.second.as<len_info>());
    }
    return ans;
}

tech::tech(const std::string &tech_fname) {
    YAML::Node node = YAML::LoadFile(tech_fname);

    tech_lib = node["tech_lib"].as<std::string>();
    layout_unit = node["layout_unit"].as<double_t>();
    resolution = node["resolution"].as<double_t>();
    use_flip_parity = node["use_flip_parity"].as<bool>();
    make_pin_obj = node["make_pin_obj"].as<bool>();

    lp_map = lp_lookup(node);
    vlookup = via_lookup(node, lp_map);

    // populate space map
    sp_map_grp.emplace(space_type::DIFF_COLOR, make_space_map(node["sp_min"], lp_map));
    sp_map_grp.emplace(space_type::LINE_END, make_space_map(node["sp_le_min"], lp_map));

    auto sp_sc_node = node["sp_sc_min"];
    if (sp_sc_node.IsDefined()) {
        sp_map_grp.emplace(space_type::SAME_COLOR, make_space_map(sp_sc_node, lp_map));
        sp_sc_type = space_type::SAME_COLOR;
    } else {
        sp_sc_type = space_type::DIFF_COLOR;
    }

    // populate len_map
    len_map = make_len_map(node["len_min"], lp_map);
}

const std::string &tech::get_tech_lib() const { return tech_lib; }

double_t tech::get_layout_unit() const { return layout_unit; }

double_t tech::get_resolution() const { return resolution; }

bool tech::get_use_flip_parity() const { return use_flip_parity; }

purp_t tech::get_default_purpose() const { return lp_map.get_default_purpose(); }

purp_t tech::get_pin_purpose() const { return lp_map.get_pin_purpose(); }

bool tech::get_make_pin() const { return make_pin_obj; }

std::string tech::get_purpose_name(purp_t purp_id) const {
    return lp_map.get_purpose_name(purp_id);
}

std::optional<lay_t> tech::get_layer_id(const std::string &layer) const {
    return lp_map.get_layer_id(layer);
}

std::optional<purp_t> tech::get_purpose_id(const std::string &purpose) const {
    return lp_map.get_purpose_id(purpose);
}

offset_t tech::get_min_space(layer_t key, offset_t width, space_type sp_type) const {
    auto map_iter = sp_map_grp.find((sp_type == space_type::SAME_COLOR) ? sp_sc_type : sp_type);
    if (map_iter == sp_map_grp.end())
        throw std::out_of_range(
            fmt::format("Cannot find space type {}", static_cast<enum_t>(sp_type)));

    const auto &cur_map = map_iter->second;
    auto vec_iter = cur_map.find(key);
    if (vec_iter == cur_map.end())
        return 0;

    const auto &w_sp_list = vec_iter->second;

    for (const auto &[w_spec, sp] : w_sp_list) {
        if (width <= w_spec)
            return sp;
    }
    return w_sp_list[w_sp_list.size() - 1].second;
}

offset_t tech::get_min_length(layer_t key, offset_t width, bool even) const {
    auto iter = len_map.find(key);
    if (iter == len_map.end())
        return 0;

    return iter->second.get_min_length(width, even);
}

via_lay_purp_t tech::get_via_layer_purpose(const std::string &key) const {
    return vlookup.get_via_layer_purpose(key);
}

via_param tech::get_via_param(vector dim, layer_t bot_layer, layer_t top_layer, orient_2d bot_dir,
                              orient_2d top_dir, bool extend) const {
    return vlookup.get_via_param(dim, bot_layer, top_layer, bot_dir, top_dir, extend);
}

} // namespace layout
} // namespace cbag
