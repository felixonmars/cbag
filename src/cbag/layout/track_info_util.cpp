#include <cstdlib>

#include <fmt/core.h>

#include <cbag/layout/track_info_util.h>
#include <cbag/layout/wire_info.h>

namespace cbag {
namespace layout {

int coord_to_htr(const track_info &tr_info, offset_t coord) {
    auto p2 = tr_info.get_pitch() / 2;
    auto off = tr_info.get_offset();
    auto delta = coord - off;
    auto div_res = std::div(delta, p2);
    auto q = div_res.quot;
    auto r = div_res.rem;
    if (r != 0)
        throw std::invalid_argument(
            fmt::format("Coordinate {} is not on track.  (p2 = {}, off = {})", coord, p2, off));

    return q;
}

offset_t htr_to_coord(const track_info &tr_info, int htr) noexcept {
    return (tr_info.get_pitch() / 2) * htr + tr_info.get_offset();
}

cnt_t get_min_space_htr(const track_info &tr_info, const tech &t, int level, cnt_t num_tr,
                        bool same_color, bool even) {
    auto p2 = tr_info.get_pitch() / 2;
    auto winfo = tr_info.get_wire_info(num_tr);
    auto span = tr_info.get_wire_span(num_tr);
    auto extra = (span - winfo.get_total_width(p2)) / 2;
    auto margin =
        std::max(0, winfo.get_min_space(t, level, get_space_type(same_color), even) - extra);

    return tr_info.space_to_htr(margin);
}

} // namespace layout
} // namespace cbag
