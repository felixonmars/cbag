#include <cbag/layout/geometry.h>

namespace cbag {
namespace layout {

union_view geometry::make_union_view() {
    switch (mode) {
    case 0:
        return rectangle_view(rect_set);
    case 1:
        return polygon90_view(poly90_set, rect_set);
    case 2:
        return polygon45_view(poly45_set, poly90_set, rect_set);
    default:
        return polygon_view(poly_set, poly45_set, poly90_set, rect_set);
    }
}

} // namespace layout
} // namespace cbag
