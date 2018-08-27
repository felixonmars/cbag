#ifndef CBAG_LAYOUT_BOUNDARY_H
#define CBAG_LAYOUT_BOUNDARY_H

#include <algorithm>
#include <unordered_map>
#include <utility>
#include <variant>

#include <boost/functional/hash.hpp>

#include <cbag/common/transform.h>
#include <cbag/layout/blockage.h>
#include <cbag/layout/boundary.h>
#include <cbag/layout/geometry.h>
#include <cbag/layout/via.h>

namespace cbag {
namespace layout {

using geo_map_t = std::unordered_map<layer_t, geometry, boost::hash<layer_t>>;

using block_map_t = std::unordered_map<layer_t, std::vector<blockage>, boost::hash<layer_t>>;

// class forwarding
class cellview;

class instance {
  public:
    inline instance(std::string lib, std::string cell, std::string view, transform xform,
                    uint32_t nx = 1, uint32_t ny = 1, coord_t spx = 0, coord_t spy = 0)
        : lib(std::move(lib)), cell(std::move(cell)), view(std::move(view)), xform(xform), nx(nx),
          ny(ny), spx(spx), spy(spy), master(nullptr) {}

  private:
    std::string lib;
    std::string cell;
    std::string view;
    transform xform;
    uint32_t nx;
    uint32_t ny;
    coord_t spx;
    coord_t spy;
    cellview *master;
};

using inst_map_t = std::unordered_map<std::string, instance>;

class cellview {
  public:
    cellview(std::string tech, uint8_t geo_mode = 0)
        : tech(std::move(tech)), inst_name_cnt(0), geo_mode(geo_mode) {}

  private:
    std::string tech;
    geo_map_t geo_map;
    block_map_t lay_block_map;
    std::vector<via> via_list;
    std::vector<polygon> area_block_list;
    std::vector<boundary> boundary_list;
    inst_map_t inst_list;
    uint32_t inst_name_cnt;
    uint8_t geo_mode;
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
