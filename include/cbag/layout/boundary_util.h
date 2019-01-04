#ifndef CBAG_LAYOUT_BOUNDARY_UTIL_H
#define CBAG_LAYOUT_BOUNDARY_UTIL_H

#include <cbag/common/box_t.h>
#include <cbag/layout/boundary.h>

namespace cbag {
namespace layout {

boundary from_box(boundary_type type, const box_t &box);

} // namespace layout
} // namespace cbag

#endif
