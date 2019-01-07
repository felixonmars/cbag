#ifndef CBAG_LAYOUT_TRACK_INFO_UTIL_H
#define CBAG_LAYOUT_TRACK_INFO_UTIL_H

#include <cbag/layout/track_info.h>

namespace cbag {
namespace layout {

offset_t get_pitch2(const track_info &tr_info);

int coord_to_track(const track_info &tr_info, offset_t coord);

} // namespace layout
} // namespace cbag

#endif
