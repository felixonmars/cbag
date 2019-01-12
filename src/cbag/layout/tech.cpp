#include <algorithm>
#include <limits>

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
    auto node = YAML::LoadFile(tech_fname);

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

    // get level-to-layer/purpose mapping
    auto tmp = cbagyaml::int_map_to_vec<std::vector<std::pair<std::string, std::string>>>(
        node["lay_purp_list"]);

    grid_bot_layer = std::get<0>(tmp);
    lp_list.reserve(std::get<1>(tmp).size());
    for (const auto &cur_lp_vec : std::get<1>(tmp)) {
        std::vector<layer_t> new_lp_vec;
        new_lp_vec.reserve(cur_lp_vec.size());
        for (const auto &[lay_str, purp_str] : cur_lp_vec) {
            auto lid = lp_map.get_layer_id(lay_str);
            if (!lid)
                throw std::out_of_range("Cannot find layer ID for layer: " + lay_str);

            auto pid = lp_map.get_purpose_id(purp_str);
            if (!pid)
                throw std::out_of_range("Cannot find purpose ID for purpose: " + purp_str);
            new_lp_vec.push_back(std::make_pair(*lid, *pid));
        }
        lp_list.push_back(new_lp_vec);
    }

    // get layer/purpose-to-level mapping
    auto lay = grid_bot_layer;
    for (const auto &lay_purp_vec : lp_list) {
        for (const auto &lp : lay_purp_vec) {
            lev_map.emplace(lp, lay);
        }
        ++lay;
    }
}

const std::string &tech::get_tech_lib() const { return tech_lib; }

double_t tech::get_layout_unit() const { return layout_unit; }

double_t tech::get_resolution() const { return resolution; }

bool tech::get_use_flip_parity() const { return use_flip_parity; }

purp_t tech::get_default_purpose() const { return lp_map.get_default_purpose(); }

purp_t tech::get_pin_purpose() const { return lp_map.get_pin_purpose(); }

bool tech::get_make_pin() const { return make_pin_obj; }

const std::string &tech::get_layer_name(lay_t lay_id) const {
    return lp_map.get_layer_name(lay_id);
}

const std::string &tech::get_purpose_name(purp_t purp_id) const {
    return lp_map.get_purpose_name(purp_id);
}

std::optional<lay_t> tech::get_layer_id(const std::string &layer) const {
    return lp_map.get_layer_id(layer);
}

std::optional<purp_t> tech::get_purpose_id(const std::string &purpose) const {
    return lp_map.get_purpose_id(purpose);
}

std::optional<int> tech::get_level(layer_t key) const {
    std::optional<int> ans;
    auto iter = lev_map.find(key);
    if (iter == lev_map.end())
        return ans;
    ans = iter->second;
    return ans;
}

const std::vector<layer_t> &tech::get_lay_purp_list(int_t level) const {
    auto idx = static_cast<std::size_t>(level - grid_bot_layer);
    if (idx >= lp_list.size())
        throw std::out_of_range("Undefined routing grid level: " + std::to_string(level));
    return lp_list[idx];
}

offset_t tech::get_min_space(layer_t key, offset_t width, space_type sp_type, bool even) const {
    auto map_iter = sp_map_grp.find((sp_type == space_type::SAME_COLOR) ? sp_sc_type : sp_type);
    if (map_iter == sp_map_grp.end())
        throw std::out_of_range("Min space not defined forspace type: " +
                                std::to_string(static_cast<enum_t>(sp_type)));

    const auto &cur_map = map_iter->second;
    auto vec_iter = cur_map.find(key);
    if (vec_iter == cur_map.end())
        return 0;

    const auto &w_sp_list = vec_iter->second;

    for (const auto &[w_spec, sp] : w_sp_list) {
        if (width <= w_spec)
            return sp + (sp & static_cast<int>(even));
    }
    auto ans = w_sp_list[w_sp_list.size() - 1].second;
    return ans + (ans & static_cast<int>(even));
}

offset_t tech::get_min_length(layer_t key, offset_t width, bool even) const {
    auto iter = len_map.find(key);
    if (iter == len_map.end())
        return 0;

    return iter->second.get_min_length(width, even);
}

const std::string &tech::get_via_id(direction vdir, layer_t layer, layer_t adj_layer) const {
    return vlookup.get_via_id(vdir, layer, adj_layer);
}

via_lay_purp_t tech::get_via_layer_purpose(const std::string &key) const {
    return vlookup.get_via_layer_purpose(key);
}

via_param tech::get_via_param(vector dim, const std::string &via_id, direction vdir,
                              orient_2d ex_dir, orient_2d adj_ex_dir, bool extend) const {
    return vlookup.get_via_param(dim, via_id, vdir, ex_dir, adj_ex_dir, extend);
}

std::tuple<double, double, double>
tech::get_metal_em_specs(const std::string &layer, offset_t width, const std::string &purpose,
                         offset_t length, bool vertical, int_t dc_temp, int_t rms_dt) const {
    constexpr auto inf = std::numeric_limits<double>::infinity();
    return {inf, inf, inf};
}

} // namespace layout
} // namespace cbag
