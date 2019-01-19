#ifndef CBAG_LAYOUT_CELLVIEW_UTIL_H
#define CBAG_LAYOUT_CELLVIEW_UTIL_H

#include <cbag/enum/min_len_mode.h>
#include <cbag/layout/cellview.h>

namespace cbag {
namespace layout {

class track_id;
class wire_array;

template <typename T> class cv_obj_ref;

box_t get_bbox(const cellview &cv, const std::string &layer, const std::string &purpose);

void add_pin(cellview &cv, const std::string &layer, const std::string &net,
             const std::string &label, const box_t &bbox);

void add_pin_arr(cellview &cv, const std::string &net, const std::string &label,
                 const wire_array &warr);

cv_obj_ref<via_wrapper> add_via(cellview &cv, transformation xform, std::string via_id,
                                const via_param &params, bool add_layers, bool commit);

void add_via_arr(cellview &cv, const transformation &xform, const std::string &via_id,
                 const via_param &params, bool add_layers, std::array<cnt_t, 2> num_arr,
                 std::array<offset_t, 2> sp_arr);

std::array<std::array<coord_t, 2>, 2> add_via_on_intersections(cellview &cv, const track_id &tid1,
                                                               const track_id &tid2,
                                                               std::array<coord_t, 2> coord1,
                                                               std::array<coord_t, 2> coord2,
                                                               bool extend, bool contain);

std::array<std::array<coord_t, 2>, 2> add_via_on_intersections(cellview &cv,
                                                               const wire_array &warr1,
                                                               const wire_array &warr2, bool extend,
                                                               bool contain);

std::array<std::array<coord_t, 2>, 2>
connect_box_track(cellview &cv, direction vdir, layer_t key, const box_t &box,
                  std::array<cnt_t, 2> num, std::array<offset_t, 2> sp, const track_id &tid,
                  const std::array<std::optional<coord_t>, 2> &w_ext,
                  const std::array<std::optional<coord_t>, 2> &tr_ext, min_len_mode mode);

std::array<std::array<coord_t, 2>, 2>
connect_warr_track(cellview &cv, const wire_array &warr, const track_id &tid,
                   const std::array<std::optional<coord_t>, 2> &w_ext,
                   const std::array<std::optional<coord_t>, 2> &tr_ext);

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
