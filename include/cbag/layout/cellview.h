#ifndef CBAG_LAYOUT_CELLVIEW_H
#define CBAG_LAYOUT_CELLVIEW_H

#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/functional/hash.hpp>

#include <cbag/layout/geometry.h>
#include <cbag/layout/instance.h>
#include <cbag/layout/typedefs.h>

namespace cbag {
namespace layout {

class pt_vector;
class polygon;
class boundary;
class blockage;
class pin;
class via;
class rectangle;
class path_ref;
class polygon90;
class polygon45;
class polygon;
template <typename T> class vector_obj_ref;
class tech;

using layer_t = std::pair<lay_t, purp_t>;
using geo_map_t = std::unordered_map<layer_t, geometry, boost::hash<layer_t>>;
using block_map_t = std::unordered_map<lay_t, std::vector<blockage>>;
using pin_map_t = std::unordered_map<lay_t, std::vector<pin>>;
using inst_map_t = std::unordered_map<std::string, instance>;
using inst_iter_t = inst_map_t::iterator;

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

    bool empty() const {
        return geo_map.empty() && inst_map.empty() && via_list.empty() && lay_block_map.empty() &&
               area_block_list.empty() && boundary_list.empty();
    }

    rectangle get_bbox(const char *layer, const char *purpose) const;

    vector_obj_ref<rectangle> add_rect(const char *layer, const char *purpose, coord_t xl,
                                       coord_t yl, coord_t xh, coord_t yh);

    vector_obj_ref<rectangle> add_rect(lay_t lay_id, purp_t purp_id, coord_t xl, coord_t yl,
                                       coord_t xh, coord_t yh);

    void add_pin(const char *layer, coord_t xl, coord_t yl, coord_t xh, coord_t yh, const char *net,
                 const char *label);

    vector_obj_ref<polygon90> add_poly90(const char *layer, const char *purpose, pt_vector data);

    vector_obj_ref<polygon45> add_poly45(const char *layer, const char *purpose, pt_vector data);

    vector_obj_ref<polygon> add_poly(const char *layer, const char *purpose, pt_vector data);

    vector_obj_ref<blockage> add_blockage(const char *layer, uint8_t blk_code, pt_vector data);

    vector_obj_ref<boundary> add_boundary(uint8_t bnd_code, pt_vector data);

    path_ref add_path(const char *layer, const char *purpose, const pt_vector &data,
                      offset_t half_width, uint8_t style0, uint8_t style1, uint8_t stylem);

    path_ref add_path45_bus(const char *layer, const char *purpose, const pt_vector &data,
                            const std::vector<offset_t> &widths,
                            const std::vector<offset_t> &spaces, uint8_t style0, uint8_t style1,
                            uint8_t stylem);

    inst_iter_t add_prim_instance(const char *lib, const char *cell, const char *view,
                                  const char *name, transformation xform, uint32_t nx, uint32_t ny,
                                  offset_t spx, offset_t spy);

    inst_iter_t add_instance(const cellview *cv, const char *name, transformation xform,
                             uint32_t nx, uint32_t ny, offset_t spx, offset_t spy);

    vector_obj_ref<via> add_via(transformation xform, const char *via_id, const uint32_t (&num)[2],
                                const dist_t (&cut_dim)[2], const offset_t (&cut_sp)[2],
                                const offset_t (&lay1_enc)[2], const offset_t (&lay1_off)[2],
                                const offset_t (&lay2_enc)[2], const offset_t (&lay2_off)[2],
                                bool add_layers);
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
