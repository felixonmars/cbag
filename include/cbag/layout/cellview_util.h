#ifndef CBAG_LAYOUT_CELLVIEW_UTIL_H
#define CBAG_LAYOUT_CELLVIEW_UTIL_H

#include <cbag/layout/cellview.h>

namespace cbag {
namespace layout {

box_t get_bbox(const cellview &cv, const std::string &layer, const std::string &purpose);

} // namespace layout
} // namespace cbag

#endif
