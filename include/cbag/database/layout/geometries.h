#ifndef CBAG_DATABASE_LAYOUT_GEOMETRIES_H
#define CBAG_DATABASE_LAYOUT_GEOMETRIES_H

#include <variant>

#include <cbag/database/layout/datatypes.h>

namespace cbag {
namespace layout {

// -----------------------------------------------------------------------------
// union views variant declaration
// -----------------------------------------------------------------------------

using UnionViewVariant = std::variant<RectView, Poly90View, Poly45View, PolyView>;

template <typename T> class PolyRef {};

/** A class representing layout geometries on the same layer.
 */
class Geometry {
  public:
    inline explicit Geometry(uint8_t mode = 0)
        : rect_set(), poly90_set(), poly45_set(), poly_set(), mode(mode), view(make_union_view()) {}

    inline const UnionViewVariant &get_view() { return view; }

  private:
    UnionViewVariant make_union_view();

    RectSet rect_set;
    Polygon90Set poly90_set;
    Polygon45Set poly45_set;
    PolygonSet poly_set;
    uint8_t mode;
    UnionViewVariant view;
}; // namespace layout

} // namespace layout
} // namespace cbag

#endif // CBAG_DATABASE_LAYOUT_GEOMETRIES_H
