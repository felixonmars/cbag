#ifndef CBAG_LAYOUT_GEOMETRY_H
#define CBAG_LAYOUT_GEOMETRY_H

#include <algorithm>

#include <boost/functional/hash.hpp>

#include <cbag/layout/datatypes.h>
#include <cbag/layout/polygon_ref.h>

namespace cbag {
namespace layout {

/** A class representing layout geometries on the same layer.
 */
class geometry {
  public:
    inline explicit geometry(uint8_t mode = 0)
        : rect_set(), poly90_set(), poly45_set(), poly_set(), mode(mode), view(make_union_view()) {}

    inline void set_mode(uint8_t m) { mode = m; }

    inline const union_view &get_view() { return view; }

    inline polygon_ref<rectangle> add_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh) {
        rect_set.emplace_back(xl, yl, xh, yh);
        return {&rect_set, rect_set.size() - 1};
    }

    inline polygon_ref<polygon90> add_poly90(point_vector_t &&data) {
        mode = std::max(mode, 1_uc);
        poly90_set.emplace_back(std::move(data));
        return {&poly90_set, poly90_set.size() - 1};
    }

    inline polygon_ref<polygon45> add_poly45(point_vector_t &&data) {
        mode = std::max(mode, 2_uc);
        poly45_set.emplace_back(std::move(data));
        return {&poly45_set, poly45_set.size() - 1};
    }

    inline polygon_ref<polygon> add_poly(point_vector_t &&data) {
        mode = std::max(mode, 3_uc);
        poly_set.emplace_back(std::move(data));
        return {&poly_set, poly_set.size() - 1};
    }

  private:
    union_view make_union_view();

    rectangle_set rect_set;
    polygon90_set poly90_set;
    polygon45_set poly45_set;
    polygon_set poly_set;
    uint8_t mode;
    union_view view;
};

} // namespace layout
} // namespace cbag

#endif
