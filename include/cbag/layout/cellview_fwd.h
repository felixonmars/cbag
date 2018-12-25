#ifndef CBAG_LAYOUT_CELLVIEW_FWD_H
#define CBAG_LAYOUT_CELLVIEW_FWD_H

#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/functional/hash.hpp>

#include <cbag/common/layer_t.h>
#include <cbag/common/transformation_fwd.h>
#include <cbag/enum/geometry_mode.h>
#include <cbag/layout/geometry.h>
#include <cbag/layout/instance.h>

namespace cbag {

class box_t;

namespace layout {

class boundary;
class blockage;
class pin;
class via;
class tech;
template <typename T> class shape_ref;
template <typename T> class cv_obj_ref;
class via_ref;
class geo_iterator;

using geo_map_t = std::unordered_map<layer_t, geometry, boost::hash<layer_t>>;
using block_map_t = std::unordered_map<lay_t, std::vector<blockage>>;
using pin_map_t = std::unordered_map<lay_t, std::vector<pin>>;
using inst_map_t = std::unordered_map<std::string, instance>;

class cellview {
  private:
    cnt_t inst_name_cnt = 0;
    geometry_mode geo_mode = geometry_mode::POLY90;
    tech *tech_ptr = nullptr;
    std::string cell_name;
    geo_map_t geo_map;
    inst_map_t inst_map;
    pin_map_t pin_map;
    std::vector<via> via_list;
    block_map_t lay_block_map;
    std::vector<blockage> area_block_list;
    std::vector<boundary> boundary_list;

    struct helper;

  public:
    explicit cellview(tech *tech_ptr, std::string cell_name,
                      geometry_mode geo_mode = geometry_mode::POLY90);

    void set_geometry_mode(geometry_mode new_mode);

    geometry &get_geometry(layer_t key);

    std::string name() const;
    tech *get_tech() const;

    bool empty() const;

    layer_t get_lay_purp_key(const std::string &layer, const std::string &purpose) const;

    box_t get_bbox(const std::string &layer, const std::string &purpose) const;

    geo_iterator begin_intersect(const layer_t &key, const box_t &r, offset_t spx,
                                 offset_t spy) const;

    geo_iterator end_intersect() const;

    auto begin_inst() const -> decltype(inst_map.cbegin());
    auto end_inst() const -> decltype(inst_map.cend());
    auto begin_geometry() const -> decltype(geo_map.cbegin());
    auto end_geometry() const -> decltype(geo_map.cend());
    auto begin_via() const -> decltype(via_list.cbegin());
    auto end_via() const -> decltype(via_list.cend());
    auto begin_lay_block() const -> decltype(lay_block_map.cbegin());
    auto end_lay_block() const -> decltype(lay_block_map.cend());
    auto begin_area_block() const -> decltype(area_block_list.cbegin());
    auto end_area_block() const -> decltype(area_block_list.cend());
    auto begin_boundary() const -> decltype(boundary_list.cbegin());
    auto end_boundary() const -> decltype(boundary_list.cend());
    auto begin_pin() const -> decltype(pin_map.cbegin());
    auto end_pin() const -> decltype(pin_map.cend());

    void add_pin(const std::string &layer, std::string net, std::string label, box_t bbox);

    shape_ref<box_t> add_rect(const std::string &layer, const std::string &purpose, bool is_horiz,
                              box_t bbox, bool commit);

    void add_rect_arr(const std::string &layer, const std::string &purpose, const box_t &box,
                      bool is_horiz, cnt_t nx, cnt_t ny, offset_t spx, offset_t spy);

    shape_ref<polygon90> add_poly90(const std::string &layer, const std::string &purpose,
                                    bool is_horiz, polygon90 &&poly, bool commit);

    shape_ref<polygon45> add_poly45(const std::string &layer, const std::string &purpose,
                                    bool is_horiz, polygon45 &&poly, bool commit);

    shape_ref<polygon45_set> add_poly45_set(const std::string &layer, const std::string &purpose,
                                            bool is_horiz, polygon45_set &&poly, bool commit);

    shape_ref<polygon> add_poly(const std::string &layer, const std::string &purpose, bool is_horiz,
                                polygon &&poly, bool commit);

    cv_obj_ref<blockage> add_blockage(blockage &&data, bool commit);

    cv_obj_ref<boundary> add_boundary(boundary &&data, bool commit);

    cv_obj_ref<via> add_via(transformation xform, std::string via_id, bool add_layers,
                            bool bot_horiz, bool top_horiz, cnt_t vnx, cnt_t vny, dist_t w,
                            dist_t h, offset_t vspx, offset_t vspy, offset_t enc1l, offset_t enc1r,
                            offset_t enc1t, offset_t enc1b, offset_t enc2l, offset_t enc2r,
                            offset_t enc2t, offset_t enc2b, bool commit);

    void add_via_arr(const transformation &xform, const std::string &via_id, bool add_layers,
                     bool bot_horiz, bool top_horiz, cnt_t vnx, cnt_t vny, dist_t w, dist_t h,
                     offset_t vspx, offset_t vspy, offset_t enc1l, offset_t enc1r, offset_t enc1t,
                     offset_t enc1b, offset_t enc2l, offset_t enc2r, offset_t enc2t, offset_t enc2b,
                     cnt_t nx, cnt_t ny, offset_t spx, offset_t spy);

    cv_obj_ref<instance> add_prim_instance(std::string lib, std::string cell, std::string view,
                                           std::string name, transformation xform, cnt_t nx,
                                           cnt_t ny, offset_t spx, offset_t spy, bool commit);

    cv_obj_ref<instance> add_instance(const cellview *cv, std::string name, transformation xform,
                                      cnt_t nx, cnt_t ny, offset_t spx, offset_t spy, bool commit);

    void add_object(const blockage &obj);
    void add_object(const boundary &obj);
    void add_object(const via &obj);
    void add_object(const instance &obj);
};

} // namespace layout
} // namespace cbag

#endif
