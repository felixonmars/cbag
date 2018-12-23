#include <cbag/common/transformation.h>

namespace cbag {

transformation::transformation(coord_t dx, coord_t dy, orientation orient)
    : bp::transformation<coord_t>(orient) {
    auto tmp = bp::axis_transformation(orient);
    tmp.invert().transform(dx, dy);
    set_translation(bp::point_data<coord_t>(-dx, -dy));
}

transformation::transformation(coord_t dx, coord_t dy, uint32_t mode)
    : transformation(dx, dy, static_cast<orientation>(mode)) {}

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

bool transformation::flips_xy() const { return (orient_code() & 0b100) == 0b100; }

std::pair<int8_t, int8_t> transformation::axis_scale() const {
    auto ocode = orient_code();
    return {1 - ((ocode & 0b001) << 1), 1 - (ocode & 0b010)};
}

transformation &transformation::move_by(offset_t dx, offset_t dy) {
    coord_t x, y;
    get_location(x, y);
    set_location(x + dx, y + dy);
    return *this;
}

transformation transformation::get_move_by(offset_t dx, offset_t dy) const {
    return transformation(*this).move_by(dx, dy);
}

transformation &transformation::transform_by(const transformation &xform) {
    operator+=(xform);
    return *this;
}

transformation transformation::get_transform_by(const transformation &xform) const {
    return transformation(*this).transform_by(xform);
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

} // namespace cbag
