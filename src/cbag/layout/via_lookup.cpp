#include <fmt/core.h>

#include <yaml-cpp/yaml.h>

#include "yaml-cpp/tuple.h"
#include "yaml-cpp/unordered_map.h"

#include <cbag/layout/tech_util.h>
#include <cbag/layout/via_lookup.h>
#include <cbag/layout/via_param_util.h>
#include <cbag/yaml/via_info.h>

namespace cbag {
namespace layout {

const std::string &via_id_at(const vid_map_t &id_map, layer_t bot_lay, layer_t top_lay) {
    auto iter = id_map.find(std::make_pair(std::move(bot_lay), std::move(top_lay)));
    if (iter == id_map.end())
        throw std::out_of_range(fmt::format("Cannot find via ID between ({}, {}) and ({}, {})",
                                            bot_lay.first, bot_lay.second, top_lay.first,
                                            top_lay.second));
    return iter->second;
}

const std::vector<via_info> &via_info_at(const vinfo_map_t &info_map, const std::string &via_id) {
    auto iter = info_map.find(via_id);
    if (iter == info_map.end())
        throw std::out_of_range("Cannot find via infor for via ID: " + via_id);

    return iter->second;
}

vlayers_t parse_via_layers(const YAML::Node &node, const lp_lookup &lp) {
    return {layer_t_at(lp, node[0][0].as<std::string>(), node[0][1].as<std::string>()),
            layer_t_at(lp, node[1][0].as<std::string>(), node[1][1].as<std::string>())};
}

via_lookup::via_lookup() = default;

via_lookup::via_lookup(const YAML::Node &parent, const lp_lookup &lp)
    : lp_map(parent["via_layers"].as<vlp_map_t>()), info_map(parent["via"].as<vinfo_map_t>()) {
    for (const auto &node : parent["via_id"]) {
        id_map.emplace(parse_via_layers(node.first, lp), node.second.as<std::string>());
    }
}

via_lay_purp_t via_lookup::get_via_layer_purpose(const std::string &key) const {
    auto iter = lp_map.find(key);
    if (iter == lp_map.end()) {
        throw std::out_of_range(fmt::format("Cannot find via ID: {}", key));
    }
    return iter->second;
}

uint64_t get_via_score(const via_param &p) {
    return static_cast<uint64_t>(p.num[0]) * p.num[1] * p.cut_dim[0] * p.cut_dim[1];
}

via_param via_lookup::get_via_param(vector dim, layer_t bot_layer, layer_t top_layer,
                                    orient_2d bot_dir, orient_2d top_dir, bool extend) const {

    auto via_id = via_id_at(id_map, std::move(bot_layer), std::move(top_layer));
    auto vinfo_list = via_info_at(info_map, via_id);

    via_param ans;
    auto opt_score = static_cast<uint64_t>(0);
    vector opt_ext_dim = {0, 0};
    for (const auto &vinfo : vinfo_list) {
        auto via_param = vinfo.get_via_param(dim, bot_dir, top_dir, extend);
        auto cur_score = get_via_score(via_param);
        vector cur_ext_dim = {get_metal_dim(via_param, bot_dir, 0),
                              get_metal_dim(via_param, top_dir, 1)};
        if (cur_score > opt_score ||
            (cur_score > 0 && cur_score == opt_score && cur_ext_dim[0] <= opt_ext_dim[0] &&
             cur_ext_dim[1] <= opt_ext_dim[1])) {
            ans = std::move(via_param);
            opt_score = cur_score;
            opt_ext_dim = cur_ext_dim;
        }
    }

    return ans;
}

} // namespace layout
} // namespace cbag
