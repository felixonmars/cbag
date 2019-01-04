
#ifndef CBAG_COMMON_TRANSFORMATION_UTIL_H
#define CBAG_COMMON_TRANSFORMATION_UTIL_H

#include <tuple>
#include <utility>

#include <cbag/common/transformation_fwd.h>
#include <cbag/enum/orientation.h>

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
void set_orient_code(transformation &xform, orient_t orient);

bool flips_xy(const transformation &xform);
std::pair<bool, bool> axis_scale(const transformation &xform);

transformation &move_by(transformation &xform, offset_t dx, offset_t dy);
transformation get_move_by(transformation xform, offset_t dx, offset_t dy);
transformation &invert(transformation &xform);
transformation get_invert(transformation xform);
transformation &transform_by(transformation &xform, const transformation &rhs);
transformation get_transform_by(transformation xform, const transformation &rhs);

std::tuple<cnt_t, cnt_t, offset_t, offset_t> convert_array(const transformation &xform, cnt_t nx,
                                                           cnt_t ny, offset_t spx, offset_t spy);

std::tuple<cnt_t, cnt_t, offset_t, offset_t> convert_gds_array(const transformation &xform,
                                                               cnt_t gds_nx, cnt_t gds_ny,
                                                               offset_t gds_spx, offset_t gds_spy);

} // namespace cbag

#endif
