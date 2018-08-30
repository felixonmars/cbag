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

} // namespace layout
} // namespace cbag
