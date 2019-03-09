#ifndef CBAG_LAYOUT_TECH_H
#define CBAG_LAYOUT_TECH_H

#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/container_hash/hash.hpp>

#include <cbag/common/layer_t.h>
#include <cbag/common/typedefs.h>
#include <cbag/enum/direction.h>
#include <cbag/enum/space_type.h>
#include <cbag/layout/len_info.h>
#include <cbag/layout/lp_lookup.h>
#include <cbag/layout/via_lookup.h>

namespace cbag {
namespace layout {

using sp_map_t =
    std::unordered_map<layer_t, std::vector<std::pair<offset_t, offset_t>>, boost::hash<layer_t>>;
using sp_map_grp_t = std::unordered_map<space_type, sp_map_t>;
using len_map_t = std::unordered_map<layer_t, len_info, boost::hash<layer_t>>;
using lp_list_t = std::vector<std::vector<layer_t>>;
using level_map_t = std::unordered_map<layer_t, level_t, boost::hash<layer_t>>;
using color_map_t = std::unordered_map<lay_t, std::pair<lay_t, cnt_t>>;

class tech {
  private:
    std::string tech_lib;
    double_t layout_unit = 1e-6;
    double_t resolution = 0.001;
    bool use_flip_parity = false;
    bool make_pin_obj = true;
    lp_lookup lp_map;
    via_lookup vlookup;
    sp_map_grp_t sp_map_grp;
    space_type sp_sc_type;
    len_map_t len_map;
    level_t grid_bot_layer = 0;
    lp_list_t lp_list;
    level_map_t lev_map;
    color_map_t color_map;

  public:
    tech(const std::string &tech_fname);

    virtual ~tech() = default;
    tech(tech &&) = default;
    tech &operator=(tech &&) = default;
    // disable copying; tech object should be singleton
    tech(const tech &) = delete;
    tech &operator=(const tech &) = delete;

    const std::string &get_tech_lib() const;

    double_t get_layout_unit() const;

    double_t get_resolution() const;

    bool get_use_flip_parity() const;

    purp_t get_default_purpose() const;

    purp_t get_pin_purpose() const;

    bool get_make_pin() const;

    const color_map_t &get_color_map() const;

    const std::string &get_layer_name(lay_t lay_id) const;

    const std::string &get_purpose_name(purp_t purp_id) const;

    std::optional<lay_t> get_layer_id(const std::string &layer) const;

    std::optional<purp_t> get_purpose_id(const std::string &purpose) const;

    std::optional<level_t> get_level(layer_t key) const;

    const std::vector<layer_t> &get_lay_purp_list(level_t level) const;

    offset_t get_min_space(layer_t key, offset_t width, space_type sp_type, bool even) const;

    offset_t get_min_length(layer_t key, offset_t width, bool even) const;

    const std::string &get_via_id(direction vdir, layer_t layer, layer_t adj_layer) const;

    via_lay_purp_t get_via_layer_purpose(const std::string &key) const;

    via_param get_via_param(vector dim, const std::string &via_id, direction vdir, orient_2d ex_dir,
                            orient_2d adj_ex_dir, bool extend) const;

    virtual em_specs_t get_metal_em_specs(const std::string &layer, const std::string &purpose,
                                          offset_t width, offset_t length, bool vertical,
                                          temp_t dc_temp, temp_t rms_dt) const;

    virtual em_specs_t get_via_em_specs(enum_t layer_dir, const std::string &layer,
                                        const std::string &purpose, const std::string &adj_layer,
                                        const std::string &adj_purpose, offset_t cut_w,
                                        offset_t cut_h, offset_t m_w, offset_t m_l,
                                        offset_t adj_m_w, offset_t adj_m_l, bool array,
                                        temp_t dc_temp, temp_t rms_dt) const;
};

} // namespace layout
} // namespace cbag

#endif
