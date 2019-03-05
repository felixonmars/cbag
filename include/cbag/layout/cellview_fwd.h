#ifndef CBAG_LAYOUT_CELLVIEW_FWD_H
#define CBAG_LAYOUT_CELLVIEW_FWD_H

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/container_hash/hash.hpp>

#include <cbag/common/layer_t.h>
#include <cbag/common/transformation_fwd.h>
#include <cbag/enum/geometry_mode.h>
#include <cbag/layout/geo_index.h>
#include <cbag/layout/geometry.h>
#include <cbag/layout/instance.h>

namespace cbag {

class box_t;

namespace layout {

class boundary;
class blockage;
class pin;
class via;
class via_wrapper;
class routing_grid;
class tech;
class label;
class track_id;

using geo_map_t = std::unordered_map<layer_t, geometry, boost::hash<layer_t>>;
using block_map_t = std::unordered_map<lay_t, std::vector<blockage>>;
using pin_map_t = std::unordered_map<lay_t, std::vector<pin>>;
using inst_map_t = std::unordered_map<std::string, instance>;

class cellview {
  private:
    cnt_t inst_name_cnt = 0;
    geometry_mode geo_mode = geometry_mode::POLY90;
    std::shared_ptr<const routing_grid> grid_ptr = nullptr;
    std::string cell_name;
    std::vector<geo_index> index_list;
    geo_map_t geo_map;
    inst_map_t inst_map;
    pin_map_t pin_map;
    std::vector<via> via_list;
    block_map_t lay_block_map;
    std::vector<blockage> area_block_list;
    std::vector<boundary> boundary_list;
    std::vector<label> label_list;

    struct helper;

  public:
    cellview(std::shared_ptr<const routing_grid> grid, std::string cell_name,
             geometry_mode geo_mode = geometry_mode::POLY90);

    bool operator==(const cellview &rhs) const noexcept;

    void set_geometry_mode(geometry_mode new_mode);

    auto find_geometry(layer_t key) const -> decltype(geo_map.find(key));

    const std::string &get_name() const noexcept;
    const tech *get_tech() const noexcept;
    const routing_grid *get_grid() const noexcept;

    bool empty() const noexcept;

    const geo_index &get_geo_index(level_t lev) const;
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
    auto begin_label() const -> decltype(label_list.cbegin());
    auto end_label() const -> decltype(label_list.cend());

    void add_pin(lay_t lay_id, std::string &&net, std::string &&label, box_t &&bbox);

    void add_label(layer_t &&key, transformation &&xform, std::string &&label, offset_t height);

    void add_object(const blockage &obj);
    void add_object(const boundary &obj);
    void add_object(boundary &&obj);
    void add_object(const via_wrapper &obj);
    void add_object(const instance &obj);
    void add_shape(layer_t key, const box_t &obj);
    void add_shape(layer_t key, const polygon90 &obj);
    void add_shape(layer_t key, const polygon45 &obj);
    void add_shape(layer_t key, const polygon &obj);
    void add_shape(layer_t key, const polygon45_set &obj);
    void add_warr(const track_id &tid, std::array<offset_t, 2> coord);
};

} // namespace layout
} // namespace cbag

#endif
