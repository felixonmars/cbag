#include <fmt/core.h>

#include <cbag/layout/via_lookup.h>

namespace cbag {
namespace layout {

via_lookup::via_lookup() = default;

via_lookup::via_lookup(vlp_map_t &&lp_map) : lp_map(std::move(lp_map)) {}

via_lay_purp_t via_lookup::get_via_layer_purpose(const std::string &key) const {
    auto iter = lp_map.find(key);
    if (iter == lp_map.end()) {
        throw std::out_of_range(fmt::format("Cannot find via ID: {}", key));
    }
    return iter->second;
}

} // namespace layout
} // namespace cbag
