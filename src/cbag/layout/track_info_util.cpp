#include <cstdlib>

#include <fmt/core.h>

#include <cbag/layout/track_info_util.h>

namespace cbag {
namespace layout {

offset_t get_pitch2(const track_info &tr_info) { return tr_info.get_pitch() >> 1; }

int coord_to_track(const track_info &tr_info, offset_t coord) {
    auto p2 = get_pitch2(tr_info);
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

} // namespace layout
} // namespace cbag
