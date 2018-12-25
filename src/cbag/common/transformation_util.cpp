#include <cbag/common/transformation.h>
#include <cbag/common/transformation_util.h>

namespace cbag {

transformation make_xform(coord_t dx, coord_t dy, orientation orient) {
    transformation ans(orient);
    auto tmp = bp::axis_transformation(orient);
    tmp.invert().transform(dx, dy);
    ans.set_translation(bp::point_data<coord_t>(-dx, -dy));
    return ans;
}

std::pair<coord_t, coord_t> location(const transformation &xform) {
    auto ans = std::make_pair<coord_t, coord_t>(0, 0);
    xform.transform(ans.first, ans.second);
    return ans;
}
coord_t x(const transformation &xform) { return location(xform).first; }
coord_t y(const transformation &xform) { return location(xform).second; }
orient_t orient_code(const transformation &xform) {
    bp::direction_2d hdir, vdir;
    xform.get_directions(hdir, vdir);
    return ((~vdir.to_int() & 0b11) << 1) | (~hdir.to_int() & 0b01);
}
orientation orient(const transformation &xform) {
    return static_cast<orientation>(orient_code(xform));
}

void set_location(transformation &xform, coord_t x, coord_t y) {
    auto atr = xform.get_axis_transformation();
    atr.invert().transform(x, y);
    xform.set_translation(bp::point_data<coord_t>(-x, -y));
}
void set_orient(transformation &xform, orientation orient) {
    // get origin
    auto loc = location(xform);
    // set new orientation
    xform.set_axis_transformation(bp::axis_transformation(orient));
    // set origin
    set_location(xform, loc.first, loc.second);
}
void set_orient_code(transformation &xform, uint32_t code) {
    set_orient(xform, static_cast<orientation>(code));
}

bool flips_xy(const transformation &xform) { return (orient_code(xform) & 0b100) == 0b100; }
std::pair<bool, bool> axis_scale(const transformation &xform) {
    auto ocode = orient_code(xform);
    return {(ocode & 0b001) == 0b001, (ocode & 0b010) == 0b010};
}

transformation &move_by(transformation &xform, offset_t dx, offset_t dy) {
    auto loc = location(xform);
    set_location(xform, loc.first + dx, loc.second + dy);
    return xform;
}
transformation get_move_by(transformation xform, offset_t dx, offset_t dy) {
    return move_by(xform, dx, dy);
}

transformation &invert(transformation &xform) {
    xform.invert();
    return xform;
}
transformation get_invert(transformation xform) { return invert(xform); }

transformation &transform_by(transformation &xform, const transformation &rhs) {
    xform += rhs;
    return xform;
}
transformation get_transform_by(transformation xform, const transformation &rhs) {
    return transform_by(xform, rhs);
}

} // namespace cbag
