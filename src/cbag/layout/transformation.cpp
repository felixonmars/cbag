#include <cbag/common/orientation.h>
#include <cbag/layout/transformation.h>

namespace cbag {
namespace layout {

transformation::transformation() = default;

transformation::transformation(bp::axis_transformation::ATR orient)
    : bp::transformation<coord_t>(orient) {}

transformation::transformation(coord_t dx, coord_t dy)
    : bp::transformation<coord_t>(bp::point_data<coord_t>(-dx, -dy)) {}

transformation::transformation(coord_t dx, coord_t dy, bp::axis_transformation::ATR orient)
    : bp::transformation<coord_t>(orient) {
    auto tmp = bp::axis_transformation(orient);
    tmp.invert().transform(dx, dy);
    set_translation(bp::point_data<coord_t>(-dx, -dy));
}

transformation::transformation(coord_t dx, coord_t dy, unsigned int mode)
    : bp::transformation<coord_t>(static_cast<bp::axis_transformation::ATR>((mode))) {
    auto tmp = bp::axis_transformation(static_cast<bp::axis_transformation::ATR>((mode)));
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

bp::axis_transformation::ATR transformation::orient() const {
    bp::direction_2d hdir, vdir;
    get_directions(hdir, vdir);
    unsigned int code = ((~vdir.to_int() & 0b11) << 1) | (~hdir.to_int() & 0b01);
    return static_cast<bp::axis_transformation::ATR>(code);
}

cbag::orientation convert_orient(bp::axis_transformation::ATR orient) {
    switch (orient) {
    case bp::axis_transformation::ATR::NULL_TRANSFORM:
        return oR0;
    case bp::axis_transformation::ATR::FLIP_X:
        return oMY;
    case bp::axis_transformation::ATR::FLIP_Y:
        return oMX;
    case bp::axis_transformation::ATR::FLIP_XY:
        return oR180;
    case bp::axis_transformation::ATR::SWAP_XY:
        return oMXR90;
    case bp::axis_transformation::ATR::ROTATE_LEFT:
        return oR270;
    case bp::axis_transformation::ATR::ROTATE_RIGHT:
        return oR90;
    default:
        return oMYR90;
    }
}

cbag::transform transformation::to_transform() const {
    coord_t x = 0, y = 0;
    transform(x, y);

    return {x, y, convert_orient(orient())};
}

} // namespace layout
} // namespace cbag
