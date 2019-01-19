
#ifndef CBAG_COMMON_BOX_T_UTIL_H
#define CBAG_COMMON_BOX_T_UTIL_H

#include <optional>

#include <cbag/common/box_t.h>
#include <cbag/common/transformation_fwd.h>
#include <cbag/common/vector.h>

namespace cbag {

offset_t get_dim(const box_t &box, orient_2d orient);
coord_t get_center(const box_t &box, orient_2d orient);

void set(box_t &box, coord_t xl, coord_t yl, coord_t xh, coord_t yh);
box_t &set_interval(box_t &box, orient_2d orient, coord_t tl, coord_t th);

coord_t xl(const box_t &box);
coord_t yl(const box_t &box);
coord_t xh(const box_t &box);
coord_t yh(const box_t &box);
coord_t xm(const box_t &box);
coord_t ym(const box_t &box);
offset_t width(const box_t &box);
offset_t height(const box_t &box);
vector dim(const box_t &box);
coord_t lower(const box_t &box, orient_2d orient);
coord_t upper(const box_t &box, orient_2d orient);

std::string to_string(const box_t &box);
bool is_physical(const box_t &box);
bool is_valid(const box_t &box);
bool overlaps(const box_t &box, const box_t &other);

box_t &merge(box_t &box, const box_t &other);
box_t get_merge(box_t box, const box_t &other);
box_t &intersect(box_t &box, const box_t &other);
box_t get_intersect(box_t box, const box_t &other);
box_t &extend_orient(box_t &box, orient_2d orient, const std::optional<coord_t> &ct,
                     const std::optional<coord_t> &cp);
box_t get_extend_orient(box_t box, orient_2d orient, const std::optional<coord_t> &ct,
                        const std::optional<coord_t> &cp);
box_t &extend(box_t &box, const std::optional<coord_t> &x, const std::optional<coord_t> &y);
box_t get_extend(box_t box, const std::optional<coord_t> &x, const std::optional<coord_t> &y);
box_t &expand(box_t &box, coord_t dx = 0, coord_t dy = 0);
box_t get_expand(box_t box, coord_t dx = 0, coord_t dy = 0);
box_t &transform(box_t &box, const transformation &xform);
box_t get_transform(box_t box, const transformation &xform);
box_t &move_by_orient(box_t &box, orient_2d orient, offset_t dt = 0, offset_t dp = 0);
box_t get_move_by_orient(box_t box, orient_2d orient, offset_t dt = 0, offset_t dp = 0);
box_t &move_by(box_t &box, offset_t dx = 0, offset_t dy = 0);
box_t get_move_by(box_t box, offset_t dx = 0, offset_t dy = 0);
box_t &flip_xy(box_t &box);
box_t get_flip_xy(box_t box);

} // namespace cbag

#endif
