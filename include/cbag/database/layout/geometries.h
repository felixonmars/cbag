#ifndef CBAG_DATABASE_LAYOUT_GEOMETRIES_H
#define CBAG_DATABASE_LAYOUT_GEOMETRIES_H

#include <algorithm>
#include <variant>

#include <cbag/database/layout/datatypes.h>

namespace cbag {
namespace layout {

// custom unsigned char literal definition
inline constexpr unsigned char operator "" _uc( unsigned long long arg ) noexcept
{
    return static_cast< unsigned char >( arg );
}

// -----------------------------------------------------------------------------
// union views variant declaration
// -----------------------------------------------------------------------------

using UnionViewVariant = std::variant<RectView, Poly90View, Poly45View, PolyView>;

template <typename T> class PolyRef {
  public:
    inline PolyRef(std::vector<T> *vec, std::size_t idx) : vec(vec), idx(idx) {}

    inline T &value() { return (*vec)[idx]; }

  private:
    std::vector<T> *vec;
    std::size_t idx;
};

/** A class representing layout geometries on the same layer.
 */
class Geometry {
  public:
    inline explicit Geometry(uint8_t mode = 0)
        : rect_set(), poly90_set(), poly45_set(), poly_set(), mode(mode), view(make_union_view()) {}

    inline const UnionViewVariant &get_view() { return view; }

    inline PolyRef<Rect> add_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh) {
        rect_set.emplace_back(xl, yl, xh, yh);
        return {&rect_set, rect_set.size() - 1};
    }

    inline PolyRef<Polygon90> add_poly90(point_vector_t &&data) {
        mode = std::max(mode, 1_uc);
        poly90_set.emplace_back(std::move(data));
        return {&poly90_set, poly90_set.size() - 1};
    }

    inline PolyRef<Polygon45> add_poly45(point_vector_t &&data) {
        mode = std::max(mode, 2_uc);
        poly45_set.emplace_back(std::move(data));
        return {&poly45_set, poly45_set.size() - 1};
    }

    inline PolyRef<Polygon> add_poly(point_vector_t &&data) {
        mode = std::max(mode, 3_uc);
        poly_set.emplace_back(std::move(data));
        return {&poly_set, poly_set.size() - 1};
    }

  private:
    UnionViewVariant make_union_view();

    RectSet rect_set;
    Polygon90Set poly90_set;
    Polygon45Set poly45_set;
    PolygonSet poly_set;
    uint8_t mode;
    UnionViewVariant view;
}; // namespace layout

struct LayCellView {

};

} // namespace layout
} // namespace cbag

#endif // CBAG_DATABASE_LAYOUT_GEOMETRIES_H
