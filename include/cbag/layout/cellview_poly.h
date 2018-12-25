#ifndef CBAG_LAYOUT_CELLVIEW_POLY_H
#define CBAG_LAYOUT_CELLVIEW_POLY_H

#include <cbag/layout/blockage.h>
#include <cbag/layout/boundary.h>
#include <cbag/layout/cv_obj_ref.h>
#include <cbag/layout/polygon45_fwd.h>
#include <cbag/layout/polygon90_fwd.h>
#include <cbag/layout/polygon_fwd.h>
#include <cbag/layout/pt_list.h>
#include <cbag/layout/tech_util.h>

namespace cbag {
namespace layout {

shape_ref<box_t> add_rect(cellview &cv, const std::string &layer, const std::string &purpose,
                          bool is_horiz, box_t bbox, bool commit);

void add_rect_arr(cellview &cv, const std::string &layer, const std::string &purpose,
                  const box_t &box, bool is_horiz, cnt_t nx, cnt_t ny, offset_t spx, offset_t spy);

template <typename T>
shape_ref<T> add_polygon(cellview &cv, const std::string &layer, const std::string &purpose,
                         bool is_horiz, T &&poly, bool commit) {
    auto key = get_layer_t(*cv.get_tech(), layer, purpose);
    return {&cv, std::move(key), is_horiz, std::forward<T>(poly), commit};
}

template <typename T, typename = IsPtList<T>>
shape_ref<polygon90> add_poly90(cellview &cv, const std::string &layer, const std::string &purpose,
                                bool is_horiz, const T &data, bool commit) {
    polygon90 poly;
    poly.set(traits::pt_list<T>::begin(data), traits::pt_list<T>::end(data));
    return add_polygon(cv, layer, purpose, is_horiz, std::move(poly), commit);
}

template <typename T, typename = IsPtList<T>>
shape_ref<polygon45> add_poly45(cellview &cv, const std::string &layer, const std::string &purpose,
                                bool is_horiz, const T &data, bool commit) {
    polygon45 poly;
    poly.set(traits::pt_list<T>::begin(data), traits::pt_list<T>::end(data));
    return add_polygon(cv, layer, purpose, is_horiz, std::move(poly), commit);
}

template <typename T, typename = IsPtList<T>>
shape_ref<polygon> add_poly(cellview &cv, const std::string &layer, const std::string &purpose,
                            bool is_horiz, const T &data, bool commit) {
    polygon poly;
    poly.set(traits::pt_list<T>::begin(data), traits::pt_list<T>::end(data));
    return add_polygon(cv, layer, purpose, is_horiz, std::move(poly), commit);
}

template <typename T, typename = IsPtList<T>>
cv_obj_ref<blockage> add_blockage(cellview &cv, const std::string &layer, enum_t blk_code,
                                  const T &data, bool commit) {
    auto lay_id = cv.get_tech()->get_layer_id(layer);
    blockage obj(static_cast<blockage_type>(blk_code), lay_id);
    obj.set(traits::pt_list<T>::begin(data), traits::pt_list<T>::end(data));
    return cv.add_blockage(std::move(obj), commit);
}

template <typename T, typename = IsPtList<T>>
cv_obj_ref<boundary> add_boundary(cellview &cv, enum_t bnd_code, const T &data, bool commit) {
    boundary obj(static_cast<boundary_type>(bnd_code));
    obj.set(traits::pt_list<T>::begin(data), traits::pt_list<T>::end(data));
    return cv.add_boundary(std::move(obj), commit);
}

} // namespace layout
} // namespace cbag

#endif
