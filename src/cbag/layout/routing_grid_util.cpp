#include <fmt/core.h>

#include <cbag/common/box_t_util.h>
#include <cbag/enum/space_type.h>
#include <cbag/layout/polygon.h>
#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/track_info_util.h>
#include <cbag/layout/via_param_util.h>
#include <cbag/layout/wire_width.h>
#include <cbag/util/binary_iterator.h>

namespace cbag {
namespace layout {

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const box_t &obj) {
    auto tech_ptr = grid.get_tech();
    auto level = tech_ptr->get_level(key);
    std::array<offset_t, 2> ans{0, 0};
    if (level) {
        auto dir = grid.track_info_at(*level).get_direction();
        auto pdir = perpendicular(dir);
        auto width = get_dim(obj, pdir);
        ans[to_int(dir)] = tech_ptr->get_min_space(key, width, space_type::LINE_END, false);
        ans[to_int(pdir)] = tech_ptr->get_min_space(key, width, space_type::DIFF_COLOR, false);
    }
    return ans;
}

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon90 &obj) {
    // TODO: add margins around polygon?
    return {0, 0};
}

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon45 &obj) {
    // TODO: add margins around polygon?
    return {0, 0};
}

std::array<offset_t, 2> get_margins(const routing_grid &grid, layer_t key, const polygon &obj) {
    // TODO: add margins around polygon?
    return {0, 0};
}

int get_lower_orthogonal_level(const routing_grid &grid, int_t level) {
    auto top_dir = grid.track_info_at(level).get_direction();
    auto bot_lev = level - 1;
    for (; bot_lev >= grid.get_bot_level(); --bot_lev) {
        if (grid[bot_lev].get_direction() != top_dir)
            return bot_lev;
    }
    return bot_lev;
}

bool block_defined_at(const routing_grid &grid, int_t level) {
    auto bot_lev = get_lower_orthogonal_level(grid, level);
    return bot_lev >= grid.get_bot_level() && bot_lev > grid.get_top_private_level();
}

std::array<offset_t, 2> get_top_track_pitches(const routing_grid &grid, int_t level) {
    if (!block_defined_at(grid, level))
        throw std::invalid_argument("Size is undefined at layer " + std::to_string(level));

    auto bot_lev = get_lower_orthogonal_level(grid, level);
    auto &tinfo = grid[level];
    auto &binfo = grid[bot_lev];

    std::array<offset_t, 2> ans;
    auto tidx = to_int(tinfo.get_direction());
    ans[tidx] = tinfo.get_pitch();
    ans[1 - tidx] = binfo.get_pitch();
    return ans;
}

std::array<offset_t, 2> get_via_extensions(const routing_grid &grid, direction vdir, int_t level,
                                           cnt_t ntr, cnt_t adj_ntr) {
    auto adj_level = get_adj_level(vdir, level);
    auto &tr_info = grid.track_info_at(level);
    auto &adj_tr_info = grid.track_info_at(adj_level);
    auto dir = tr_info.get_direction();
    auto adj_dir = adj_tr_info.get_direction();
    if (dir == adj_dir) {
        // TODO: implement for layers with same direction
        throw std::invalid_argument(
            "get_via_extensions() right now requires two layers with different directions.");
    }
    auto &tech = *grid.get_tech();
    auto wire_width = tr_info.get_wire_width(ntr);
    auto adj_wire_width = adj_tr_info.get_wire_width(adj_ntr);
    auto key = get_test_lay_purp(tech, level);
    auto adj_key = get_test_lay_purp(tech, adj_level);
    auto &via_id = tech.get_via_id(vdir, key, adj_key);
    vector vbox_dim;
    vbox_dim[to_int(dir)] = wire_width.get_edge_wire_width();
    vbox_dim[to_int(adj_dir)] = adj_wire_width.get_edge_wire_width();
    auto via_param = tech.get_via_param(vbox_dim, via_id, vdir, dir, adj_dir, true);

    if (empty(via_param)) {
        auto vidx = to_int(vdir);
        std::array<int, 2> lev_arr;
        lev_arr[vidx] = level;
        lev_arr[1 - vidx] = adj_level;
        std::array<cnt_t, 2> ntr_arr;
        ntr_arr[vidx] = ntr;
        ntr_arr[1 - vidx] = adj_ntr;
        throw std::invalid_argument(
            fmt::format("Cannot draw via with bot_level={}, bot_ntr={}, top_ntr={}", lev_arr[0],
                        ntr_arr[0], ntr_arr[1]));
    }

    return get_via_extensions(via_param, vbox_dim, vdir, dir, adj_dir);
}

offset_t get_line_end_space_htr(const routing_grid &grid, direction vdir, int_t level, cnt_t ntr) {
    auto sp_level = get_adj_level(vdir, level);
    auto &tr_info = grid.track_info_at(level);
    if (tr_info.get_direction() == grid.track_info_at(sp_level).get_direction())
        throw std::invalid_argument("space layer must be orthogonal to wire layer.");

    auto via_ext = get_via_extensions(grid, vdir, level, ntr, 1)[to_int(vdir)];
    auto wire_w = tr_info.get_wire_width(ntr);
    auto sp_le = get_min_space(*grid.get_tech(), level, wire_w, space_type::LINE_END, false);
    return tr_info.space_to_htr(2 * via_ext + sp_le);
}

std::array<offset_t, 2> get_blk_size(const routing_grid &grid, int_t level, bool include_private,
                                     std::array<bool, 2> half_blk) {
    auto xidx = static_cast<int>(half_blk[0]);
    auto yidx = static_cast<int>(half_blk[1]);

    // default quantization is 2 if no half-block, 1 if half-block.
    std::array<offset_t, 2> ans = {2 - xidx, 2 - yidx};

    auto &top_info = grid.track_info_at(level);
    auto top_dir = top_info.get_direction();
    auto top_didx = to_int(top_dir);
    ans[1 - top_didx] = top_info.get_blk_pitch(half_blk[1 - top_didx]);

    // find bot level track info
    auto bot_lev = get_lower_orthogonal_level(grid, level);
    if (bot_lev >= grid.get_bot_level()) {
        auto private_lev = grid.get_top_private_level();
        if (include_private || level <= private_lev || private_lev < bot_lev) {
            // bottom level is quantized only if:
            // 1. include_private flag is enabled.
            // 2. top and bottom level are both public or both private.
            ans[top_didx] = grid[bot_lev].get_blk_pitch(half_blk[top_didx]);
        }
    }
    return ans;
}

layer_t get_layer_t(const routing_grid &grid, int_t level, int_t htr) {
    auto idx = grid.get_htr_parity(level, htr);
    return grid.get_tech()->get_lay_purp_list(level)[idx];
}

std::array<offset_t, 2> get_wire_bounds(const routing_grid &grid, int_t level, int_t htr,
                                        cnt_t ntr) {
    auto &tinfo = grid.track_info_at(level);
    auto wire_w = tinfo.get_wire_width(ntr);
    auto half_w = wire_w.get_total_width(tinfo.get_pitch()) / 2;
    auto coord = htr_to_coord(tinfo, htr);
    return std::array<offset_t, 2>{coord - half_w, coord + half_w};
}

em_specs_t get_wire_em_specs(const routing_grid &grid, int_t level, cnt_t ntr, offset_t length,
                             bool vertical, int_t dc_temp, int_t rms_dt) {
    auto wire_w = grid.track_info_at(level).get_wire_width(ntr);
    auto &tech = *grid.get_tech();
    return get_metal_em_specs(tech, level, wire_w, length, vertical, dc_temp, rms_dt);
}

em_specs_t get_via_em_specs(const routing_grid &grid, direction vdir, int_t level,
                            const wire_width &wire_w, offset_t length, const wire_width &adj_wire_w,
                            offset_t adj_length, int_t dc_temp, int_t rms_dt) {
    auto &tech = *grid.get_tech();
    auto adj_level = get_adj_level(vdir, level);
    auto &tinfo = grid.track_info_at(level);
    auto &adj_tinfo = grid.track_info_at(adj_level);

    auto tr_dir = tinfo.get_direction();
    auto adj_tr_dir = adj_tinfo.get_direction();

    if (tr_dir == adj_tr_dir) {
        std::array<int_t, 2> lev_vec;
        lev_vec[to_int(vdir)] = level;
        lev_vec[1 - to_int(vdir)] = adj_level;

        throw std::invalid_argument(fmt::format("levels ({}, {}) have the same direction, but "
                                                "get_via_em_specs only works on orthogonal levels.",
                                                lev_vec[0], lev_vec[1]));
    }

    auto tdir_idx = to_int(tr_dir);
    auto key = get_test_lay_purp(tech, level);
    auto adj_key = get_test_lay_purp(tech, adj_level);
    auto &via_id = tech.get_via_id(vdir, key, adj_key);
    double idc = 0, iac_rms = 0, iac_peak = 0;
    vector vbox_dim;
    std::array<offset_t, 2> m_dim, adj_m_dim;
    for (auto wi = wire_w.begin_width(), we = wire_w.end_width(); wi != we; ++wi) {
        auto w = *wi;
        vbox_dim[1 - tdir_idx] = w;
        m_dim[0] = w;
        m_dim[1] = length;
        for (auto awi = adj_wire_w.begin_width(), awe = adj_wire_w.end_width(); awi != awe; ++awi) {
            auto aw = *awi;
            vbox_dim[tdir_idx] = aw;
            adj_m_dim[0] = aw;
            adj_m_dim[1] = adj_length;

            auto param = tech.get_via_param(vbox_dim, via_id, vdir, tr_dir, adj_tr_dir, true);
            if (empty(param)) {
                // no solution: return all zeros
                return {0, 0, 0};
            }
            auto is_array = param.num[0] > 1 || param.num[1] > 1;
            auto [cur_idc, cur_irms, cur_ipeak] =
                get_via_em_specs(tech, vdir, key, adj_key, param.cut_dim, m_dim, adj_m_dim,
                                 is_array, dc_temp, rms_dt);

            idc += cur_idc;
            iac_rms += cur_irms;
            iac_peak += cur_ipeak;
        }
    }

    return {idc, iac_rms, iac_peak};
}

cnt_t get_min_num_tr(const routing_grid &grid, int_t level, double idc, double iac_rms,
                     double iac_peak, offset_t length, cnt_t bot_ntr, cnt_t top_ntr, int_t dc_temp,
                     int_t rms_dt) {
    util::binary_iterator<cnt_t> bin_iter(1);

    auto &tech = *grid.get_tech();
    auto &tr_info = grid.track_info_at(level);
    auto tr_dir = tr_info.get_direction();
    auto bot_tr_info_ptr = static_cast<const track_info *>(nullptr);
    auto top_tr_info_ptr = static_cast<const track_info *>(nullptr);
    if (bot_ntr > 0) {
        bot_tr_info_ptr = &grid.track_info_at(level - 1);
        if (bot_tr_info_ptr->get_direction() == tr_dir)
            bot_tr_info_ptr = nullptr;
    }
    if (top_ntr > 0) {
        top_tr_info_ptr = &grid.track_info_at(level + 1);
        if (top_tr_info_ptr->get_direction() == tr_dir)
            top_tr_info_ptr = nullptr;
    }
    double idc_max = 0, irms_max = 0, ipeak_max = 0;
    auto result = std::tie(idc_max, irms_max, ipeak_max);
    while (bin_iter.has_next()) {
        auto cur_ntr = *bin_iter;
        auto wire_w = tr_info.get_wire_width(cur_ntr);

        result = get_metal_em_specs(tech, level, wire_w, length, false, dc_temp, rms_dt);
        if (idc > idc_max || iac_rms > irms_max || iac_peak > ipeak_max) {
            bin_iter.up();
            continue;
        }
        // wire passes EM specs, check top/bottom via EM specs
        if (bot_tr_info_ptr) {
            auto adj_wire_w = bot_tr_info_ptr->get_wire_width(bot_ntr);
            result = get_via_em_specs(grid, direction::UPPER, level, wire_w, length, adj_wire_w, -1,
                                      dc_temp, rms_dt);
            if ((idc_max == 0 && irms_max == 0 && ipeak_max == 0) || idc > idc_max ||
                iac_rms > irms_max || iac_peak > ipeak_max) {
                bin_iter.up();
                continue;
            }
        }
        if (top_tr_info_ptr) {
            auto adj_wire_w = top_tr_info_ptr->get_wire_width(top_ntr);
            result = get_via_em_specs(grid, direction::LOWER, level, wire_w, length, adj_wire_w, -1,
                                      dc_temp, rms_dt);
            if ((idc_max == 0 && irms_max == 0 && ipeak_max == 0) || idc > idc_max ||
                iac_rms > irms_max || iac_peak > ipeak_max) {
                bin_iter.up();
                continue;
            }
        }
        // we got here, all EM specs passed.
        bin_iter.save();
        bin_iter.down();
    }

    return *bin_iter.get_save();
}

} // namespace layout
} // namespace cbag
