#include <cbag/database/layout/geometries.h>

namespace cbag {
namespace layout {

UnionViewVariant Geometry::make_union_view() {
    switch (mode) {
    case 0: {
        return RectSetView(rect_set_ptr);
    }
    case 1: {
        return Poly90View(poly90_set_ptr, rect_set_ptr);
    }
    case 2: {
        auto tmp = std::make_shared<Poly90View>(poly90_set_ptr, rect_set_ptr);
        return Poly45View(poly45_set_ptr, tmp);
    }
    default: {
        auto tmp1 = std::make_shared<PolyView1>(poly_set_ptr, poly45_set_ptr);
        auto tmp2 = std::make_shared<Poly90View>(poly90_set_ptr, rect_set_ptr);
        return PolyView(tmp1, tmp2);
    }
    }
}

} // namespace layout
} // namespace cbag
