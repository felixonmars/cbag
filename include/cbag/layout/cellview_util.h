#ifndef CBAG_LAYOUT_CELLVIEW_UTIL_H
#define CBAG_LAYOUT_CELLVIEW_UTIL_H

#include <cbag/layout/cellview.h>

namespace cbag {
namespace layout {

template <typename T> class cv_obj_ref;

box_t get_bbox(const cellview &cv, const std::string &layer, const std::string &purpose);

cv_obj_ref<via> add_via(cellview &cv, transformation xform, std::string via_id, bool add_layers,
                        bool bot_horiz, bool top_horiz, cnt_t vnx, cnt_t vny, dist_t w, dist_t h,
                        offset_t vspx, offset_t vspy, offset_t enc1l, offset_t enc1r,
                        offset_t enc1t, offset_t enc1b, offset_t enc2l, offset_t enc2r,
                        offset_t enc2t, offset_t enc2b, bool commit);

void add_via_arr(cellview &cv, const transformation &xform, const std::string &via_id,
                 bool add_layers, bool bot_horiz, bool top_horiz, cnt_t vnx, cnt_t vny, dist_t w,
                 dist_t h, offset_t vspx, offset_t vspy, offset_t enc1l, offset_t enc1r,
                 offset_t enc1t, offset_t enc1b, offset_t enc2l, offset_t enc2r, offset_t enc2t,
                 offset_t enc2b, cnt_t nx, cnt_t ny, offset_t spx, offset_t spy);

cv_obj_ref<instance> add_prim_instance(std::string lib, std::string cell, std::string view,
                                       std::string name, transformation xform, cnt_t nx, cnt_t ny,
                                       offset_t spx, offset_t spy, bool commit);

cv_obj_ref<instance> add_instance(const cellview *cv, std::string name, transformation xform,
                                  cnt_t nx, cnt_t ny, offset_t spx, offset_t spy, bool commit);

} // namespace layout
} // namespace cbag

#endif
