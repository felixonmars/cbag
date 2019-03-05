#ifndef CBAG_LAYOUT_CELLVIEW_POLY_H
#define CBAG_LAYOUT_CELLVIEW_POLY_H

#include <cbag/layout/blockage.h>
#include <cbag/layout/boundary.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/cv_obj_ref.h>
#include <cbag/layout/polygon45_fwd.h>
#include <cbag/layout/polygon90_fwd.h>
#include <cbag/layout/polygon_fwd.h>
#include <cbag/layout/pt_list.h>
#include <cbag/layout/tech_util.h>

namespace cbag {
namespace layout {

class track_id;
class wire_array;

shape_ref<box_t> add_rect(const std::shared_ptr<cellview> &cv_ptr, const std::string &layer,
                          const std::string &purpose, box_t bbox, bool commit);

void add_rect_arr(cellview &cv, layer_t &key, const box_t &box, std::array<cnt_t, 2> num,
                  std::array<offset_t, 2> sp);

void add_warr(cellview &cv, const wire_array &warr);

template <typename T>
shape_ref<T> add_polygon(const std::shared_ptr<cellview> &cv_ptr, const std::string &layer,
                         const std::string &purpose, T &&poly, bool commit) {
    auto key = layer_t_at(*(cv_ptr->get_tech()), layer, purpose);
    return {cv_ptr, std::move(key), std::forward<T>(poly), commit};
}

template <typename T, typename = IsPtList<T>>
shape_ref<polygon90> add_poly90(const std::shared_ptr<cellview> &cv_ptr, const std::string &layer,
                                const std::string &purpose, const T &data, bool commit) {
    polygon90 poly;
    poly.set(traits::pt_list<T>::begin(data), traits::pt_list<T>::end(data));
    return add_polygon(cv_ptr, layer, purpose, std::move(poly), commit);
}

template <typename T, typename = IsPtList<T>>
shape_ref<polygon45> add_poly45(const std::shared_ptr<cellview> &cv_ptr, const std::string &layer,
                                const std::string &purpose, const T &data, bool commit) {
    polygon45 poly;
    poly.set(traits::pt_list<T>::begin(data), traits::pt_list<T>::end(data));
    return add_polygon(cv_ptr, layer, purpose, std::move(poly), commit);
}

template <typename T, typename = IsPtList<T>>
shape_ref<polygon> add_poly(const std::shared_ptr<cellview> &cv_ptr, const std::string &layer,
                            const std::string &purpose, const T &data, bool commit) {
    polygon poly;
    poly.set(traits::pt_list<T>::begin(data), traits::pt_list<T>::end(data));
    return add_polygon(cv_ptr, layer, purpose, std::move(poly), commit);
}

template <typename T, typename = IsPtList<T>>
cv_obj_ref<blockage> add_blockage(const std::shared_ptr<cellview> &cv_ptr, const std::string &layer,
                                  blockage_type blk_type, const T &data, bool commit) {
    auto lay_id_opt = cv_ptr->get_tech()->get_layer_id(layer);
    auto lay_id = (lay_id_opt) ? *lay_id_opt : 0;
    blockage obj(blk_type, lay_id);
    obj.set(traits::pt_list<T>::begin(data), traits::pt_list<T>::end(data));
    return {cv_ptr, std::move(obj), commit};
}

template <typename T, typename = IsPtList<T>>
cv_obj_ref<boundary> add_boundary(const std::shared_ptr<cellview> &cv_ptr, boundary_type bnd_type,
                                  const T &data, bool commit) {
    boundary obj(bnd_type);
    obj.set(traits::pt_list<T>::begin(data), traits::pt_list<T>::end(data));
    return {cv_ptr, std::move(obj), commit};
}

} // namespace layout
} // namespace cbag

#endif
