#ifndef CBAG_LAYOUT_TECH_H
#define CBAG_LAYOUT_TECH_H

#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cbag/common/layer_t.h>
#include <cbag/common/typedefs.h>
#include <cbag/enum/space_type.h>
#include <cbag/layout/via_lookup.h>

namespace cbag {
namespace layout {

using lay_map_t = std::unordered_map<std::string, lay_t>;
using purp_map_t = std::unordered_map<std::string, purp_t>;
using sp_map_t = std::unordered_map<std::string, std::vector<std::pair<offset_t, offset_t>>>;
using sp_map_grp_t = std::unordered_map<space_type, sp_map_t>;
using lay_type_map_t = std::unordered_map<lay_t, std::string>;

class tech {
  private:
    via_lookup vlookup;
    lay_map_t lay_map;
    purp_map_t purp_map;
    sp_map_grp_t sp_map_grp;
    lay_type_map_t lay_type_map;
    purp_t default_purpose;
    space_type sp_sc_type;
    purp_t pin_purpose;
    bool make_pin_obj = true;

  public:
    tech(const std::string &tech_fname);

    purp_t get_pin_purpose() const;

    purp_t get_default_purpose() const;

    bool get_make_pin() const;

    std::string get_purpose_name(purp_t purp_id) const;

    std::optional<lay_t> get_layer_id(const std::string &layer) const;

    std::optional<purp_t> get_purpose_id(const std::string &purpose) const;

    std::string get_layer_type(lay_t lay_id) const;

    offset_t get_min_space(const std::string &layer_type, offset_t width, space_type sp_type) const;

    via_lay_purp_t get_via_layer_purpose(const std::string &key) const;
};

} // namespace layout
} // namespace cbag

#endif
