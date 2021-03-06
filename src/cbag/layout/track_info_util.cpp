#include <cstdlib>

#include <fmt/core.h>

#include <cbag/common/transformation_util.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/track_info_util.h>
#include <cbag/layout/wire_width.h>
#include <cbag/util/math.h>

namespace cbag {
namespace layout {

htr_t coord_to_htr(offset_t coord, offset_t pitch, offset_t off, round_mode mode, bool even) {
    auto modulus = pitch >> (1 - even);
    auto delta = coord - off;
    auto div_res = std::div(delta, modulus);
    auto q = div_res.quot;
    auto r = div_res.rem;
    switch (mode) {
    case round_mode::LESS:
        q -= (r == 0);
        break;
    case round_mode::LESS_EQ:
        break;
    case round_mode::NEAREST: {
        auto mod2 = modulus / 2;
        // round to nearest, if exactly in middle,
        // (meaning p2 is even and r == p4), round to even.
        q += (r > mod2 || (r == mod2 && (modulus & 1) == 0 && ((q + 1) & 1) == 0));
        break;
    }
    case round_mode::GREATER_EQ:
        q += (r != 0);
        break;
    case round_mode::GREATER:
        q += (r == 0);
        break;
    default:
        if (r != 0)
            throw std::invalid_argument(fmt::format(
                "Coordinate {} is not on track.  (pitch = {}, off = {})", coord, modulus, off));
    }
    return q;
}

htr_t coord_to_htr(const track_info &tr_info, offset_t coord, round_mode mode, bool even) {
    return coord_to_htr(coord, tr_info.get_pitch(), tr_info.get_offset(), mode, even);
}

htr_t find_next_htr(const track_info &tr_info, offset_t coord, cnt_t ntr, round_mode mode,
                    bool even) {
    auto int_mode = static_cast<senum_t>(mode);
    if (mode == round_mode::NEAREST || mode == round_mode::NONE)
        throw std::invalid_argument("Invalid find_next_htr rounding mode: " +
                                    std::to_string(int_mode));
    auto sgn = util::sign(int_mode);
    auto wire_w = tr_info.get_wire_width(ntr).get_total_width(tr_info.get_pitch() / 2);
    return coord_to_htr(tr_info, coord + sgn * (wire_w / 2), mode, even);
}

offset_t htr_to_coord(const track_info &tr_info, htr_t htr) noexcept {
    return (tr_info.get_pitch() / 2) * htr + tr_info.get_offset();
}

cnt_t get_min_space_htr(const track_info &tr_info, const tech &t, level_t level, cnt_t num_tr,
                        bool same_color, bool even) {
    auto p2 = tr_info.get_pitch() / 2;
    auto wire_w = tr_info.get_wire_width(num_tr);
    auto span = tr_info.get_wire_span(num_tr);
    auto extra = (span - wire_w.get_total_width(p2)) / 2;
    auto margin =
        std::max(0, get_min_space(t, level, wire_w, get_space_type(same_color), even) - extra);

    return tr_info.space_to_htr(margin);
}

htr_t transform_htr(const track_info &tr_info, htr_t htr, const transformation &xform) {
    if (flips_xy(xform))
        throw std::invalid_argument("Cannot transform track index when axes are swapped.");

    auto pdir = perpendicular(tr_info.get_direction());
    auto scale = axis_scale(xform)[to_int(pdir)];
    return scale * htr + coord_to_htr(tr_info, get_coord(xform, pdir));
}

} // namespace layout
} // namespace cbag
