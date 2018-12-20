#ifndef CBAG_LAYOUT_CELLVIEW_FWD_H
#define CBAG_LAYOUT_CELLVIEW_FWD_H

#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/functional/hash.hpp>

#include <cbag/layout/geometry.h>
#include <cbag/layout/instance.h>

namespace cbag {

class box_t;
class transformation;

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
    uint32_t inst_name_cnt = 0;
    uint8_t geo_mode = 0;
    struct helper;

  public:
    tech *tech_ptr = nullptr;
    std::string cell_name;
    geo_map_t geo_map;
    inst_map_t inst_map;
    pin_map_t pin_map;
    std::vector<via> via_list;
    block_map_t lay_block_map;
    std::vector<blockage> area_block_list;
    std::vector<boundary> boundary_list;

    explicit cellview(tech *tech_ptr, std::string cell_name, uint8_t geo_mode = 0);

    void set_geometry_mode(uint8_t new_mode);

    bool empty() const;

    layer_t get_lay_purp_key(const std::string &layer, const std::string &purpose) const;

    box_t get_bbox(const std::string &layer, const std::string &purpose) const;

    geo_iterator begin_intersect(const layer_t &key, const box_t &r, offset_t spx,
                                 offset_t spy) const;

    geo_iterator end_intersect() const;

    void add_pin(const std::string &layer, std::string net, std::string label, box_t bbox);

    shape_ref<box_t> add_rect(const std::string &layer, const std::string &purpose, bool is_horiz,
                              box_t bbox, bool commit);

    void add_rect_arr(const std::string &layer, const std::string &purpose, const box_t &box,
                      bool is_horiz, uint32_t nx, uint32_t ny, offset_t spx, offset_t spy);

    shape_ref<polygon90> add_poly90(const std::string &layer, const std::string &purpose,
                                    bool is_horiz, const pt_vector &data, bool commit);

    shape_ref<polygon45> add_poly45(const std::string &layer, const std::string &purpose,
                                    bool is_horiz, const pt_vector &data, bool commit);

    shape_ref<polygon> add_poly(const std::string &layer, const std::string &purpose, bool is_horiz,
                                const pt_vector &data, bool commit);

    shape_ref<polygon45_set> add_path(const std::string &layer, const std::string &purpose,
                                      bool is_horiz, const pt_vector &data, offset_t half_width,
                                      uint8_t style0, uint8_t style1, uint8_t stylem, bool commit);

    shape_ref<polygon45_set> add_path45_bus(const std::string &layer, const std::string &purpose,
                                            bool is_horiz, const pt_vector &data,
                                            const std::vector<offset_t> &widths,
                                            const std::vector<offset_t> &spaces, uint8_t style0,
                                            uint8_t style1, uint8_t stylem, bool commit);

    cv_obj_ref<blockage> add_blockage(const std::string &layer, uint8_t blk_code,
                                      const pt_vector &data, bool commit);

    cv_obj_ref<boundary> add_boundary(uint8_t bnd_code, const pt_vector &data, bool commit);

    cv_obj_ref<via> add_via(transformation xform, std::string via_id, bool add_layers,
                            bool bot_horiz, bool top_horiz, uint32_t vnx, uint32_t vny, dist_t w,
                            dist_t h, offset_t vspx, offset_t vspy, offset_t enc1x, offset_t enc1y,
                            offset_t off1x, offset_t off1y, offset_t enc2x, offset_t enc2y,
                            offset_t off2x, offset_t off2y, bool commit);

    void add_via_arr(const transformation &xform, const std::string &via_id, bool add_layers,
                     bool bot_horiz, bool top_horiz, uint32_t vnx, uint32_t vny, dist_t w, dist_t h,
                     offset_t vspx, offset_t vspy, offset_t enc1x, offset_t enc1y, offset_t off1x,
                     offset_t off1y, offset_t enc2x, offset_t enc2y, offset_t off2x, offset_t off2y,
                     uint32_t nx, uint32_t ny, offset_t spx, offset_t spy);

    cv_obj_ref<instance> add_prim_instance(std::string lib, std::string cell, std::string view,
                                           std::string name, transformation xform, uint32_t nx,
                                           uint32_t ny, offset_t spx, offset_t spy, bool commit);

    cv_obj_ref<instance> add_instance(const cellview *cv, std::string name, transformation xform,
                                      uint32_t nx, uint32_t ny, offset_t spx, offset_t spy,
                                      bool commit);

    void add_object(const blockage &obj);
    void add_object(const boundary &obj);
    void add_object(const via &obj);
    void add_object(const instance &obj);
};

} // namespace layout
} // namespace cbag

#endif
