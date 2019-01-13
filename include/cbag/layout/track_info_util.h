#ifndef CBAG_LAYOUT_TRACK_INFO_UTIL_H
#define CBAG_LAYOUT_TRACK_INFO_UTIL_H

#include <cbag/layout/track_info.h>

namespace cbag {
namespace layout {

htr_t coord_to_htr(const track_info &tr_info, offset_t coord);

offset_t htr_to_coord(const track_info &tr_info, htr_t htr) noexcept;

cnt_t get_min_space_htr(const track_info &tr_info, const tech &t, level_t level, cnt_t num_tr,
                        bool same_color, bool even);

} // namespace layout
} // namespace cbag

#endif
