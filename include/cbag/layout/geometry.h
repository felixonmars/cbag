#ifndef CBAG_LAYOUT_GEOMETRY_H
#define CBAG_LAYOUT_GEOMETRY_H

#include <string>
#include <unordered_map>

#include <cbag/layout/end_style.h>
#include <cbag/layout/polygon45_view.h>
#include <cbag/layout/polygon90_view.h>
#include <cbag/layout/polygon_sets.h>
#include <cbag/layout/polygon_view.h>
#include <cbag/layout/rectangle_view.h>
#include <cbag/layout/typedefs.h>
#include <cbag/layout/union_view.h>
#include <cbag/layout/vector_obj_ref.h>

namespace cbag {
namespace layout {

class pt_vector;

/** A class representing layout geometries on the same layer.
 */
class geometry {
  public:
    rectangle_set rect_set;
    polygon90_set poly90_set;
    polygon45_set poly45_set;
    polygon_set poly_set;

  private:
    uint8_t mode = 0;
    union_view view;
    struct helper;

  public:
    explicit geometry(uint8_t mode = 0);

    const union_view &get_view() const { return view; }

    rectangle get_bbox() const;

    void set_mode(uint8_t m) { mode = m; }

    vector_obj_ref<rectangle> add_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh);

    vector_obj_ref<polygon90> add_poly90(pt_vector data);

    vector_obj_ref<polygon45> add_poly45(pt_vector data);

    vector_obj_ref<polygon> add_poly(pt_vector data);

    static pt_vector path_to_poly45(coord_t x0, coord_t y0, coord_t x1, coord_t y1,
                                    offset_t half_width, const char *style0, const char *style1);
};

} // namespace layout
} // namespace cbag

#endif
