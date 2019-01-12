#include <numeric>
#include <unordered_map>

#include <fmt/core.h>

#include <yaml-cpp/yaml.h>

#include <cbag/common/transformation_util.h>
#include <cbag/layout/flip_parity.h>
#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/track_info_util.h>
#include <cbag/layout/wire_width.h>
#include <cbag/util/binary_iterator.h>
#include <cbag/util/math.h>
#include <cbag/yaml/common.h>

namespace cbag {
namespace layout {

struct routing_grid::helper {
    static std::size_t get_index(const routing_grid &grid, int_t level) {
        auto idx = static_cast<std::size_t>(level - grid.bot_level);
        if (idx >= grid.info_list.size())
            throw std::out_of_range("Undefined routing grid level: " + std::to_string(level));
        return idx;
    }

    static void update_blk_pitch_helper(std::vector<track_info> &info_list, int start, int stop) {
        using p_type = std::array<offset_t, 2>;
        using p_vec = std::vector<p_type>;
        auto num_lay = stop - start;
        std::array<p_vec, 2> pitch_list = {p_vec{p_type{1, 1}}, p_vec{p_type{1, 1}}};
        pitch_list[0].reserve(num_lay);
        pitch_list[1].reserve(num_lay);

        for (auto idx = start; idx < stop; ++idx) {
            auto &info = info_list[idx];
            info.blk_pitch[0] = info.get_pitch();
            info.blk_pitch[1] = info.blk_pitch[0] / 2;
            auto didx = static_cast<orient_2d_t>(info.dir);
            for (const auto &[bp, bp2] : pitch_list[didx]) {
                info.blk_pitch[0] = std::lcm(info.blk_pitch[0], bp);
                info.blk_pitch[1] = std::lcm(info.blk_pitch[1], bp2);
            }
            pitch_list[didx].push_back(info.blk_pitch);
        }
    }

    static void update_block_pitch(std::vector<track_info> &info_list, int bot_level,
                                   int top_private, int top_ignore) {
        // set ignore layers block pitch
        for (decltype(bot_level) idx = 0; idx <= top_ignore - bot_level; ++idx) {
            info_list[idx].blk_pitch = std::array<offset_t, 2>{-1, -1};
        }

        // set private layers block pitch
        auto start = top_ignore + 1 - bot_level;
        auto stop = top_private + 1 - bot_level;
        if (stop > start)
            update_blk_pitch_helper(info_list, start, stop);

        // set public layers block pitch
        start = std::max(start, stop);
        stop = info_list.size();
        if (stop > start)
            update_blk_pitch_helper(info_list, start, stop);
    }
};

routing_grid::routing_grid() = default;

routing_grid::routing_grid(const tech *t, const std::string &fname) : tech_ptr(t) {
    auto node = YAML::LoadFile(fname);

    auto tmp = cbagyaml::int_map_to_vec<track_info>(node["routing_grid"]);
    bot_level = std::get<0>(tmp);
    top_ignore_level = bot_level - 1;
    top_private_level = bot_level - 1;
    info_list = std::move(std::get<1>(tmp));
    helper::update_block_pitch(info_list, bot_level, top_private_level, top_ignore_level);
}

bool routing_grid::operator==(const routing_grid &rhs) const noexcept {
    return tech_ptr == rhs.tech_ptr && bot_level == rhs.bot_level && info_list == rhs.info_list &&
           top_ignore_level == rhs.top_ignore_level && top_private_level == rhs.top_private_level;
}

const track_info &routing_grid::operator[](int_t level) const {
    return info_list[static_cast<std::size_t>(level - bot_level)];
}

const tech *routing_grid::get_tech() const noexcept { return tech_ptr; }

int routing_grid::get_bot_level() const noexcept { return bot_level; }

int routing_grid::get_top_ignore_level() const noexcept { return top_ignore_level; }

int routing_grid::get_top_private_level() const noexcept { return top_private_level; }

const track_info &routing_grid::track_info_at(int_t level) const {
    auto idx = helper::get_index(*this, level);
    return info_list[idx];
}

flip_parity routing_grid::get_flip_parity_at(int bot_level, int top_level,
                                             const transformation &xform) const {
    if (flips_xy(xform))
        throw std::invalid_argument("Unsupported orientation: " +
                                    std::to_string(orient_code(xform)));

    auto ascale = axis_scale(xform);
    auto loc = location(xform);
    std::vector<std::tuple<int, offset_t, offset_t>> data;
    data.reserve(top_level - bot_level + 1);
    for (auto lev = bot_level; lev <= top_level; ++lev) {
        auto tr_info = operator[](lev);
        auto dir = tr_info.get_direction();
        auto didx = static_cast<orient_2d_t>(dir);
        auto coord = loc[didx];
        auto scale = ascale[didx];
        auto htr = coord_to_htr(tr_info, coord);

        auto cur_scale = tr_info.par_scale;
        auto cur_offset = tr_info.par_offset;
        auto new_scale = cur_scale * scale;
        auto new_offset = (cur_scale * htr + cur_offset);
        data.emplace_back(lev, new_scale, new_offset);
    }

    return flip_parity(std::move(data));
}

cnt_t routing_grid::get_htr_parity(int_t level, int_t htr) const {
    auto &tinfo = track_info_at(level);
    auto val = tinfo.par_scale * htr + tinfo.par_offset;
    auto modulus = tech_ptr->get_lay_purp_list(level).size();
    return util::pos_mod(val, 2 * modulus) / 2;
}

void routing_grid::set_flip_parity(const flip_parity &fp) {
    for (const auto &[level, scale, offset] : fp.data_) {
        auto idx = helper::get_index(*this, level);
        info_list[idx].par_scale = scale;
        info_list[idx].par_offset = offset;
    }
}

cnt_t routing_grid::get_min_num_tr(int_t level, double idc, double iac_rms, double iac_peak,
                                   offset_t length, cnt_t bot_ntr, cnt_t top_ntr, int_t dc_temp,
                                   int_t rms_dt) const {
    util::binary_iterator<cnt_t> bin_iter(1);

    auto &tech = *get_tech();
    auto &tr_info = track_info_at(level);
    auto tr_dir = tr_info.get_direction();
    auto bot_tr_info_ptr = static_cast<const track_info *>(nullptr);
    auto top_tr_info_ptr = static_cast<const track_info *>(nullptr);
    if (bot_ntr > 0) {
        bot_tr_info_ptr = &track_info_at(level - 1);
        if (bot_tr_info_ptr->get_direction() == tr_dir)
            bot_tr_info_ptr = nullptr;
    }
    if (top_ntr > 0) {
        top_tr_info_ptr = &track_info_at(level + 1);
        if (top_tr_info_ptr->get_direction() == tr_dir)
            top_tr_info_ptr = nullptr;
    }
    while (bin_iter.has_next()) {
        auto cur_ntr = *bin_iter;
        auto wire_w = tr_info.get_wire_width(cur_ntr);

        auto [idc_max, irms_max, ipeak_max] =
            get_metal_em_specs(tech, level, wire_w, length, false, dc_temp, rms_dt);
        if (idc > idc_max || iac_rms > irms_max || iac_peak > ipeak_max) {
            bin_iter.up();
        } else {
            // wire passes EM specs, check top/bottom via EM specs
            if (bot_tr_info_ptr) {
                auto adj_winfo = bot_tr_info_ptr->get_wire_width(bot_ntr);
            }
            if (top_tr_info_ptr) {
                auto adj_winfo = top_tr_info_ptr->get_wire_width(top_ntr);
            }
        }
    }

    return 1;
}

} // namespace layout
} // namespace cbag
