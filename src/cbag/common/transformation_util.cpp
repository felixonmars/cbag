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
void set_orient_code(transformation &xform, orient_t code) {
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

std::tuple<cnt_t, cnt_t, offset_t, offset_t> convert_array(const transformation &xform, cnt_t nx,
                                                           cnt_t ny, offset_t spx, offset_t spy) {
    get_invert(xform).transform(spx, spy);
    return (flips_xy(xform)) ? std::make_tuple(ny, nx, spx, spy)
                             : std::make_tuple(nx, ny, spx, spy);
}

std::tuple<cnt_t, cnt_t, offset_t, offset_t> convert_gds_array(const transformation &xform,
                                                               cnt_t gds_nx, cnt_t gds_ny,
                                                               offset_t gds_spx, offset_t gds_spy) {
    xform.transform(gds_spx, gds_spy);
    return (flips_xy(xform)) ? std::make_tuple(gds_ny, gds_nx, gds_spx, gds_spy)
                             : std::make_tuple(gds_nx, gds_ny, gds_spx, gds_spy);
}

} // namespace cbag
