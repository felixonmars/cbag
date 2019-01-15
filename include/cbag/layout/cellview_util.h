#ifndef CBAG_LAYOUT_CELLVIEW_UTIL_H
#define CBAG_LAYOUT_CELLVIEW_UTIL_H

#include <cbag/layout/cellview.h>

namespace cbag {
namespace layout {

template <typename T> class cv_obj_ref;

box_t get_bbox(const cellview &cv, const std::string &layer, const std::string &purpose);

void add_pin(cellview &cv, const std::string &layer, const std::string &net,
             const std::string &label, const box_t &bbox);

void add_pin_arr(cellview &cv, const std::string &net, const std::string &label, level_t level,
                 htr_t htr, offset_t lower, offset_t upper, cnt_t ntr, cnt_t n, offset_t htr_pitch);

cv_obj_ref<via_wrapper> add_via(cellview &cv, transformation xform, std::string via_id,
                                bool add_layers, bool bot_horiz, bool top_horiz, cnt_t vnx,
                                cnt_t vny, offset_t w, offset_t h, offset_t vspx, offset_t vspy,
                                offset_t enc1l, offset_t enc1r, offset_t enc1t, offset_t enc1b,
                                offset_t enc2l, offset_t enc2r, offset_t enc2t, offset_t enc2b,
                                bool commit);

void add_via_arr(cellview &cv, const transformation &xform, const std::string &via_id,
                 bool add_layers, bool bot_horiz, bool top_horiz, cnt_t vnx, cnt_t vny, offset_t w,
                 offset_t h, offset_t vspx, offset_t vspy, offset_t enc1l, offset_t enc1r,
                 offset_t enc1t, offset_t enc1b, offset_t enc2l, offset_t enc2r, offset_t enc2t,
                 offset_t enc2b, cnt_t nx, cnt_t ny, offset_t spx, offset_t spy);

void add_label(cellview &cv, const std::string &layer, const std::string &purpose,
               transformation xform, std::string label);

cv_obj_ref<instance> add_prim_instance(cellview &cv, std::string lib, std::string cell,
                                       std::string view, std::string name, transformation xform,
                                       cnt_t nx, cnt_t ny, offset_t spx, offset_t spy, bool commit);

cv_obj_ref<instance> add_instance(cellview &cv, const cellview *master, std::string name,
                                  transformation xform, cnt_t nx, cnt_t ny, offset_t spx,
                                  offset_t spy, bool commit);

} // namespace layout
} // namespace cbag

#endif
