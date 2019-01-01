#ifndef CBAG_LAYOUT_TECH_HELPER_H
#define CBAG_LAYOUT_TECH_HELPER_H

#include <string>

#include <cbag/common/layer_t.h>
#include <cbag/layout/tech.h>

namespace cbag {
namespace layout {

inline lay_t layer_id_at(const lay_map_t &lay_map, const std::string &layer) {
    auto iter = lay_map.find(layer);
    if (iter == lay_map.end())
        throw std::out_of_range("Cannot find layer ID for layer: " + layer);
    return iter->second;
}

inline purp_t purpose_id_at(const purp_map_t &purp_map, const std::string &purpose) {
    auto iter = purp_map.find(purpose);
    if (iter == purp_map.end())
        throw std::out_of_range("Cannot find purpose ID for purpose: " + purpose);
    return iter->second;
}

} // namespace layout
} // namespace cbag

#endif
