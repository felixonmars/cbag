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
class via_wrapper;
class tech;

using geo_map_t = std::unordered_map<layer_t, geometry, boost::hash<layer_t>>;
using block_map_t = std::unordered_map<lay_t, std::vector<blockage>>;
using pin_map_t = std::unordered_map<lay_t, std::vector<pin>>;
using inst_map_t = std::unordered_map<std::string, instance>;

class cellview {
  private:
    cnt_t inst_name_cnt = 0;
    geometry_mode geo_mode = geometry_mode::POLY90;
    const tech *tech_ptr = nullptr;
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
    explicit cellview(const tech *tech_ptr, std::string cell_name,
                      geometry_mode geo_mode = geometry_mode::POLY90);

    void set_geometry_mode(geometry_mode new_mode);

    auto find_geometry(layer_t key) const -> decltype(geo_map.find(key));
    geometry &make_geometry(layer_t key);

    const std::string &get_name() const noexcept;
    const tech *get_tech() const noexcept;

    bool empty() const noexcept;

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

    void add_object(const blockage &obj);
    void add_object(const boundary &obj);
    void add_object(const via_wrapper &obj);
    void add_object(const instance &obj);
};

} // namespace layout
} // namespace cbag

#endif
