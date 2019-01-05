#include <unordered_map>

#include <fmt/core.h>

#include <yaml-cpp/yaml.h>

#include <cbag/layout/routing_grid.h>
#include <cbag/yaml/common.h>

namespace cbag {
namespace layout {

routing_grid::routing_grid() = default;

routing_grid::routing_grid(const tech *t, const std::string &fname) : tech_ptr(t) {
    auto node = YAML::LoadFile(fname);

    auto tmp = cbagyaml::int_map_to_vec<track_info>(node["routing_grid"]);
    bot_layer = std::get<0>(tmp);
    info_list = std::move(std::get<1>(tmp));
}

bool routing_grid::operator==(const routing_grid &rhs) const noexcept {
    return tech_ptr == rhs.tech_ptr && bot_layer == rhs.bot_layer && info_list == rhs.info_list;
}

const tech *routing_grid::get_tech() const noexcept { return tech_ptr; }

orient_2d routing_grid::get_direction(int level) const {
    auto idx = static_cast<std::size_t>(level - bot_layer);
    if (idx >= info_list.size())
        throw std::out_of_range("Undefined routing grid level: " + std::to_string(level));
    return info_list[idx].get_direction();
}

} // namespace layout
} // namespace cbag
