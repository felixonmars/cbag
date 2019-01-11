#ifndef CBAG_LAYOUT_ROUTING_GRID_UTIL_H
#define CBAG_LAYOUT_ROUTING_GRID_UTIL_H

#include <array>

#include <cbag/common/layer_t.h>
#include <cbag/common/typedefs.h>
#include <cbag/enum/direction.h>
#include <cbag/layout/polygon45_fwd.h>
#include <cbag/layout/polygon45_set_fwd.h>
#include <cbag/layout/polygon90_fwd.h>
#include <cbag/layout/polygon_fwd.h>
#include <cbag/layout/routing_grid_fwd.h>

namespace cbag {

class box_t;

namespace layout {

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const box_t &obj);
std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon90 &obj);
std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon45 &obj);
std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon &obj);

std::array<offset_t, 2> get_via_extensions(const routing_grid &grid, direction vdir, int level,
                                           cnt_t ntr, cnt_t adj_ntr);

int get_lower_orthogonal_level(const routing_grid &grid, int level);

bool block_defined_at(const routing_grid &grid, int level);

std::array<offset_t, 2> get_top_track_pitches(const routing_grid &grid, int level);

offset_t get_line_end_space_htr(const routing_grid &grid, direction vdir, int level, cnt_t ntr);

std::array<offset_t, 2> get_blk_size(const routing_grid &grid, int level, bool include_private,
                                     std::array<bool, 2> half_blk);

layer_t get_layer_t(const routing_grid &grid, int level, int htr);

std::array<offset_t, 2> get_wire_bounds(const routing_grid &grid, int level, int htr, cnt_t ntr);

} // namespace layout
} // namespace cbag

#endif
