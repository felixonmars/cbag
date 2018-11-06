#include <cbag/common/transformation.h>

namespace cbag {

transformation::transformation() = default;

transformation::transformation(orientation orient) : bp::transformation<coord_t>(orient) {}

transformation::transformation(coord_t dx, coord_t dy)
    : bp::transformation<coord_t>(bp::point_data<coord_t>(-dx, -dy)) {}

transformation::transformation(coord_t dx, coord_t dy, orientation orient)
    : bp::transformation<coord_t>(orient) {
    auto tmp = bp::axis_transformation(orient);
    tmp.invert().transform(dx, dy);
    set_translation(bp::point_data<coord_t>(-dx, -dy));
}

transformation::transformation(coord_t dx, coord_t dy, uint32_t mode)
    : bp::transformation<coord_t>(static_cast<orientation>((mode))) {
    bp::axis_transformation tmp(static_cast<orientation>((mode)));
    tmp.invert().transform(dx, dy);
    set_translation(bp::point_data<coord_t>(-dx, -dy));
}

coord_t transformation::x() const {
    coord_t x = 0, y = 0;
    transform(x, y);
    return x;
}

coord_t transformation::y() const {
    coord_t x = 0, y = 0;
    transform(x, y);
    return y;
}

orientation transformation::orient() const { return static_cast<orientation>(orient_code()); }

uint32_t transformation::orient_code() const {
    bp::direction_2d hdir, vdir;
    get_directions(hdir, vdir);
    return ((~vdir.to_int() & 0b11) << 1) | (~hdir.to_int() & 0b01);
}

void transformation::get_location(coord_t &x, coord_t &y) const {
    x = y = 0;
    transform(x, y);
}

bool transformation::flip_xy() const {
    bp::direction_2d x, y;
    get_directions(x, y);
    auto code = x.to_int();
    return code == bp::direction_2d_enum::NORTH || code == bp::direction_2d_enum::SOUTH;
}

transformation transformation::get_move_by(offset_t dx, offset_t dy) const {
    coord_t x, y;
    get_location(x, y);
    return {x + dx, y + dy, orient()};
}

transformation transformation::get_inverse() const {
    transformation ans = *this;
    ans.invert();
    return ans;
}

void transformation::set_location(coord_t x, coord_t y) {
    auto atr = get_axis_transformation();
    atr.invert().transform(x, y);
    set_translation(bp::point_data<coord_t>(-x, -y));
}

void transformation::set_orient_code(uint32_t code) {
    // get origin
    coord_t x, y;
    get_location(x, y);
    // set new orientation
    set_axis_transformation(bp::axis_transformation(static_cast<orientation>(code)));
    // set origin
    set_location(x, y);
}

void transformation::move_by(offset_t dx, offset_t dy) {
    coord_t x, y;
    get_location(x, y);
    set_location(x + dx, y + dy);
}

} // namespace cbag
