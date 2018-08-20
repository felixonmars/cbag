#ifndef CBAG_DATABASE_LAYOUT_GEOMETRIES_H
#define CBAG_DATABASE_LAYOUT_GEOMETRIES_H

#include <variant>

#include <cbag/database/layout/datatypes.h>

namespace cbag {
namespace layout {

// -----------------------------------------------------------------------------
// union views variant declaration
// -----------------------------------------------------------------------------

using Poly90View = Polygon90SetUnionView<Polygon90Set, RectSet>;
using Poly45View = Polygon45SetUnionView<Polygon45Set, Poly90View>;
using PolyView1 = PolygonSetUnionView<PolygonSet, Polygon45Set>;
using PolyView = PolygonSetUnionView<PolyView1, Poly90View>;

using UnionViewVariant = std::variant<RectSetView, Poly90View, Poly45View, PolyView>;

/** A class representing layout geometries on the same layer.
 */
class Geometry {
  public:
    inline explicit Geometry(uint8_t mode = 0)
        : rect_set_ptr(std::make_shared<RectSet>()),
          poly90_set_ptr(std::make_shared<Polygon90Set>()),
          poly45_set_ptr(std::make_shared<Polygon45Set>()),
          poly_set_ptr(std::make_shared<PolygonSet>()), mode(mode), view_ptr() {
        view_ptr = std::make_unique<UnionViewVariant>(make_union_view());
    }

    inline UnionViewVariant *get_view() { return view_ptr.get(); }

  private:
    UnionViewVariant make_union_view();

    std::shared_ptr<RectSet> rect_set_ptr;
    std::shared_ptr<Polygon90Set> poly90_set_ptr;
    std::shared_ptr<Polygon45Set> poly45_set_ptr;
    std::shared_ptr<PolygonSet> poly_set_ptr;
    uint8_t mode;
    std::unique_ptr<UnionViewVariant> view_ptr;
};

} // namespace layout
} // namespace cbag

#endif // CBAG_DATABASE_LAYOUT_GEOMETRIES_H
