#ifndef CBAG_LAYOUT_ROUTING_GRID_UTIL_H
#define CBAG_LAYOUT_ROUTING_GRID_UTIL_H

#include <array>

#include <cbag/common/layer_t.h>
#include <cbag/common/typedefs.h>
#include <cbag/layout/polygon45_fwd.h>
#include <cbag/layout/polygon45_set_fwd.h>
#include <cbag/layout/polygon90_fwd.h>
#include <cbag/layout/polygon_fwd.h>
#include <cbag/layout/routing_grid.h>

namespace cbag {

class box_t;

namespace layout {

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const box_t &obj);
std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon90 &obj);
std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon45 &obj);
std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon &obj);

} // namespace layout
} // namespace cbag

#endif
