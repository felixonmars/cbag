#include <fmt/core.h>

#include <yaml-cpp/yaml.h>

#include <cbag/layout/routing_grid.h>
#include <cbag/layout/track_info.h>

namespace cbag {
namespace layout {

routing_grid::routing_grid() = default;

routing_grid::routing_grid(const tech *t, const std::string &fname) : tech_ptr(t) {
    auto node = YAML::LoadFile(fname);

    bot_layer = node["bot_layer"].as<decltype(bot_layer)>();
    info_list = node["layer_info"].as<decltype(info_list)>();
}

} // namespace layout
} // namespace cbag
