#include <cbag/layout/geometries.h>

namespace cbag {
namespace layout {

UnionViewVariant Geometry::make_union_view() {
    switch (mode) {
    case 0:
        return RectView(rect_set);
    case 1:
        return Poly90View(poly90_set, rect_set);
    case 2:
        return Poly45View(poly45_set, poly90_set, rect_set);
    default:
        return PolyView(poly_set, poly45_set, poly90_set, rect_set);
    }
}

} // namespace layout
} // namespace cbag
