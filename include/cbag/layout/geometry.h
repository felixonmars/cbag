#ifndef CBAG_LAYOUT_GEOMETRY_H
#define CBAG_LAYOUT_GEOMETRY_H

#include <cbag/layout/polygon45_view.h>
#include <cbag/layout/polygon90_view.h>
#include <cbag/layout/polygon_ref.h>
#include <cbag/layout/polygon_sets.h>
#include <cbag/layout/polygon_view.h>
#include <cbag/layout/rectangle_view.h>
#include <cbag/layout/typedefs.h>
#include <cbag/layout/union_view.h>

namespace cbag {
namespace layout {

/** A class representing layout geometries on the same layer.
 */
class geometry {
  private:
    rectangle_set rect_set;
    polygon90_set poly90_set;
    polygon45_set poly45_set;
    polygon_set poly_set;
    uint8_t mode;
    union_view view;

  public:
    explicit geometry(uint8_t mode = 0);

    const union_view &get_view() const { return view; }

    rectangle get_bbox() const;

    void set_mode(uint8_t m) { mode = m; }

    polygon_ref<rectangle> add_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh);

    polygon_ref<polygon90> add_poly90(point_vector_t data);

    polygon_ref<polygon45> add_poly45(point_vector_t data);

    polygon_ref<polygon> add_poly(point_vector_t data);

  private:
    union_view make_union_view();
};

point_vector_t path_to_poly45(coord_t x0, coord_t y0, coord_t x1, coord_t y1, offset_t width,
                              const char *style0, const char *style1);

} // namespace layout
} // namespace cbag

#endif
