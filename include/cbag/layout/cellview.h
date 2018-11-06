#ifndef CBAG_LAYOUT_CELLVIEW_H
#define CBAG_LAYOUT_CELLVIEW_H

#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/functional/hash.hpp>

#include <cbag/layout/geometry.h>
#include <cbag/layout/instance.h>

namespace cbag {

class transformation;

namespace layout {

class boundary;
class blockage;
class rectangle;
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

    explicit cellview(tech *tech_ptr, const char *cell_name, uint8_t geo_mode = 0);

    void set_geometry_mode(uint8_t new_mode);

    bool empty() const;

    layer_t get_lay_purp_key(const char *layer, const char *purpose) const;

    rectangle get_bbox(const char *layer, const char *purpose) const;

    geo_iterator begin_intersect(const layer_t &key, const rectangle &r, offset_t spx,
                                 offset_t spy) const;

    geo_iterator end_intersect() const;

    void add_pin(const char *layer, coord_t xl, coord_t yl, coord_t xh, coord_t yh, const char *net,
                 const char *label);

    shape_ref<rectangle> add_rect(const char *layer, const char *purpose, bool is_horiz, coord_t xl,
                                  coord_t yl, coord_t xh, coord_t yh, bool commit);

    shape_ref<polygon90> add_poly90(const char *layer, const char *purpose, bool is_horiz,
                                    const pt_vector &data, bool commit);

    shape_ref<polygon45> add_poly45(const char *layer, const char *purpose, bool is_horiz,
                                    const pt_vector &data, bool commit);

    shape_ref<polygon> add_poly(const char *layer, const char *purpose, bool is_horiz,
                                const pt_vector &data, bool commit);

    shape_ref<polygon45_set> add_path(const char *layer, const char *purpose, bool is_horiz,
                                      const pt_vector &data, offset_t half_width, uint8_t style0,
                                      uint8_t style1, uint8_t stylem, bool commit);

    shape_ref<polygon45_set> add_path45_bus(const char *layer, const char *purpose, bool is_horiz,
                                            const pt_vector &data,
                                            const std::vector<offset_t> &widths,
                                            const std::vector<offset_t> &spaces, uint8_t style0,
                                            uint8_t style1, uint8_t stylem, bool commit);

    cv_obj_ref<blockage> add_blockage(const char *layer, uint8_t blk_code, const pt_vector &data,
                                      bool commit);

    cv_obj_ref<boundary> add_boundary(uint8_t bnd_code, const pt_vector &data, bool commit);

    cv_obj_ref<via> add_via(transformation xform, const char *via_id, const uint32_t (&num)[2],
                            const dist_t (&cut_dim)[2], const offset_t (&cut_sp)[2],
                            const offset_t (&lay1_enc)[2], const offset_t (&lay1_off)[2],
                            const offset_t (&lay2_enc)[2], const offset_t (&lay2_off)[2],
                            bool add_layers, bool bot_horiz, bool top_horiz, bool commit);

    cv_obj_ref<instance> add_prim_instance(const char *lib, const char *cell, const char *view,
                                           const char *name, transformation xform, uint32_t nx,
                                           uint32_t ny, offset_t spx, offset_t spy, bool commit);

    cv_obj_ref<instance> add_instance(const cellview *cv, const char *name, transformation xform,
                                      uint32_t nx, uint32_t ny, offset_t spx, offset_t spy,
                                      bool commit);

    void add_object(const blockage &obj);
    void add_object(const boundary &obj);
    void add_object(const via &obj);
    void add_object(const instance &obj);
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
