
#ifndef CBAG_COMMON_TRANSFORMATION_UTIL_H
#define CBAG_COMMON_TRANSFORMATION_UTIL_H

#include <utility>

#include <cbag/common/orientation.h>
#include <cbag/common/transformation_fwd.h>

namespace bp = boost::polygon;

namespace cbag {

transformation make_xform(coord_t dx = 0, coord_t dy = 0, orientation orient = oR0);

coord_t x(const transformation &xform);
coord_t y(const transformation &xform);
std::pair<coord_t, coord_t> location(const transformation &xform);
orientation orient(const transformation &xform);
orient_t orient_code(const transformation &xform);

void set_location(transformation &xform, coord_t x, coord_t y);
void set_orient(transformation &xform, orientation orient);
void set_orient_code(transformation &xform, uint32_t orient);

bool flips_xy(const transformation &xform);
std::pair<bool, bool> axis_scale(const transformation &xform);

transformation &move_by(transformation &xform, offset_t dx, offset_t dy);
transformation get_move_by(transformation xform, offset_t dx, offset_t dy);
transformation &invert(transformation &xform);
transformation get_invert(transformation xform);
transformation &transform_by(transformation &xform, const transformation &rhs);
transformation get_transform_by(transformation xform, const transformation &rhs);

} // namespace cbag

#endif
