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

class wire_width;

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const box_t &obj);
std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon90 &obj);
std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon45 &obj);
std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon &obj);

std::array<offset_t, 2> get_via_extensions(const routing_grid &grid, direction vdir, int_t level,
                                           cnt_t ntr, cnt_t adj_ntr);

int get_lower_orthogonal_level(const routing_grid &grid, int_t level);

bool block_defined_at(const routing_grid &grid, int_t level);

std::array<offset_t, 2> get_top_track_pitches(const routing_grid &grid, int_t level);

offset_t get_line_end_space_htr(const routing_grid &grid, direction vdir, int_t level, cnt_t ntr);

std::array<offset_t, 2> get_blk_size(const routing_grid &grid, int_t level, bool include_private,
                                     std::array<bool, 2> half_blk);

layer_t get_layer_t(const routing_grid &grid, int_t level, int_t htr);

std::array<offset_t, 2> get_wire_bounds(const routing_grid &grid, int_t level, int_t htr,
                                        cnt_t ntr);

em_specs_t get_wire_em_specs(const routing_grid &grid, int_t level, cnt_t ntr, offset_t length,
                             bool vertical, int_t dc_temp, int_t rms_dt);

em_specs_t get_via_em_specs(const routing_grid &grid, direction vdir, int_t level,
                            const wire_width &wire_w, offset_t length, const wire_width &adj_wire_w,
                            offset_t adj_length, int_t dc_temp, int_t rms_dt);

cnt_t get_min_num_tr(const routing_grid &grid, int_t level, double idc, double iac_rms,
                     double iac_peak, offset_t length, cnt_t bot_ntr, cnt_t top_ntr, int_t dc_temp,
                     int_t rms_dt);

} // namespace layout
} // namespace cbag

#endif
