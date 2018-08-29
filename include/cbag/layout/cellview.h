#ifndef CBAG_LAYOUT_CELLVIEW_H
#define CBAG_LAYOUT_CELLVIEW_H

#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/functional/hash.hpp>

#include <cbag/layout/geometry.h>
#include <cbag/layout/instance.h>
#include <cbag/layout/polygon_ref.h>
#include <cbag/layout/typedefs.h>

namespace cbag {
namespace layout {

class polygon;
class boundary;
class blockage;
class via;
class rectangle;
class polygon90;
class polygon45;
class polygon;

using layer_t = std::pair<lay_t, purp_t>;
using geo_map_t = std::unordered_map<layer_t, geometry, boost::hash<layer_t>>;
using block_map_t = std::unordered_map<layer_t, std::vector<blockage>, boost::hash<layer_t>>;
using inst_map_t = std::unordered_map<std::string, instance>;

class cellview {
  private:
    std::string tech;
    geo_map_t geo_map;
    inst_map_t inst_map;
    std::vector<via> via_list;
    block_map_t lay_block_map;
    std::vector<polygon> area_block_list;
    std::vector<boundary> boundary_list;
    uint32_t inst_name_cnt;
    uint8_t geo_mode;

  public:
    explicit cellview(std::string tech, uint8_t geo_mode = 0);

    bool is_empty() const {
        return geo_map.empty() && inst_map.empty() && via_list.empty() && lay_block_map.empty() &&
               area_block_list.empty() && boundary_list.empty();
    }

    rectangle get_bbox(const layer_t &layer) const;

    std::string add_instance(instance inst, const char *name = nullptr);

    polygon_ref<rectangle> add_rect(const layer_t &layer, coord_t xl, coord_t yl, coord_t xh,
                                    coord_t yh);

    polygon_ref<polygon90> add_poly90(const layer_t &layer, point_vector_t data);

    polygon_ref<polygon45> add_poly45(const layer_t &layer, point_vector_t data);

    polygon_ref<polygon> add_poly(const layer_t &layer, point_vector_t data);

    void add_path_seg(const layer_t &layer, coord_t x0, coord_t y0, coord_t x1, coord_t y1,
                      dist_t width, const char *style0, const char *style1);

  private:
    std::string get_inst_name();

    geo_map_t::iterator get_geometry(const layer_t &layer);
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
