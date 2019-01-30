#include <fmt/core.h>

#include <cbag/common/transformation.h>
#include <cbag/common/transformation_util.h>

namespace boost {
namespace polygon {

std::ostream &operator<<(std::ostream &os, const cbag::transformation &value) {
    return os << cbag::to_string(value);
}

} // namespace polygon
} // namespace boost

namespace cbag {

std::string to_string(const transformation &xform) {
    auto loc = location(xform);
    auto ori = orient(xform);
    return fmt::format("Transform({}, {}, {})", loc[0], loc[1], to_string(ori));
}

transformation make_xform(coord_t dx, coord_t dy, orientation orient) {
    transformation ans(orient);
    auto tmp = bp::axis_transformation(orient);
    tmp.invert().transform(dx, dy);
    ans.set_translation(bp::point_data<coord_t>(-dx, -dy));
    return ans;
}

std::array<coord_t, 2> location(const transformation &xform) {
    auto ans = std::array<coord_t, 2>{0, 0};
    xform.transform(ans[0], ans[1]);
    return ans;
}

coord_t get_coord(const transformation &xform, orient_2d orient) {
    return location(xform)[static_cast<orient_2d_t>(orient)];
}

coord_t x(const transformation &xform) { return get_coord(xform, orient_2d::HORIZONTAL); }
coord_t y(const transformation &xform) { return get_coord(xform, orient_2d::VERTICAL); }
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
    set_location(xform, loc[0], loc[1]);
}
void set_orient_code(transformation &xform, orient_t code) {
    set_orient(xform, static_cast<orientation>(code));
}

bool flips_xy(const transformation &xform) { return (orient_code(xform) & 0b100) == 0b100; }
std::array<int, 2> axis_scale(const transformation &xform) {
    auto ocode = orient_code(xform);
    return {1 - 2 * static_cast<int>(ocode & 0b001),
            1 - 2 * static_cast<int>((ocode & 0b010) >> 1)};
}

transformation &move_by(transformation &xform, offset_t dx, offset_t dy) {
    auto loc = location(xform);
    set_location(xform, loc[0] + dx, loc[1] + dy);
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
    // NOTE: operator += is broken
    auto new_shift = location(xform);
    rhs.transform(new_shift[0], new_shift[1]);
    auto atr = xform.get_axis_transformation();
    atr += rhs.get_axis_transformation();
    xform.set_axis_transformation(atr);
    set_location(xform, new_shift[0], new_shift[1]);
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
