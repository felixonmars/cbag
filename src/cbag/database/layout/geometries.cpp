#include <cbag/database/layout/geometries.h>

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

PolyRef<Rect> Geometry::add_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh) {
    rect_set.emplace_back(xl, yl, xh, yh);
    return {&rect_set, rect_set.size() - 1};
}

} // namespace layout
} // namespace cbag
