#ifndef CBAG_LAYOUT_TECH_H
#define CBAG_LAYOUT_TECH_H

#include <string>
#include <unordered_map>
#include <utility>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

enum space_type : uint8_t {
    LINE_END = 0,
    DIFF_COLOR = 1,
    SAME_COLOR = 2,
};

using lay_map_t = std::unordered_map<std::string, lay_t>;
using purp_map_t = std::unordered_map<std::string, purp_t>;
using via_map_t = std::unordered_map<std::string, std::pair<lay_t, lay_t>>;
using sp_map_t =
    std::unordered_map<std::string, std::pair<std::vector<offset_t>, std::vector<offset_t>>>;
using sp_map_grp_t = std::unordered_map<space_type, sp_map_t>;
using lay_type_map_t = std::unordered_map<lay_t, std::string>;

class tech {
  private:
    lay_map_t lay_map;
    purp_map_t purp_map;
    via_map_t via_map;
    sp_map_grp_t sp_map_grp;
    lay_type_map_t lay_type_map;
    purp_t default_purpose;
    space_type sp_sc_type;

  public:
    purp_t pin_purpose;
    bool make_pin_obj = true;

    tech(const char *tech_fname);

    std::string get_pin_purpose_name() const;

    std::string get_default_purpose_name() const;

    std::string get_purpose_name(purp_t purp_id) const;

    lay_t get_layer_id(const char *layer) const;

    purp_t get_purpose_id(const char *purpose) const;

    std::string get_layer_type(lay_t lay_id) const;

    void get_via_layers(const std::string &key, lay_t &bot, lay_t &top) const;

    offset_t get_min_space(const std::string &layer_type, offset_t width, uint8_t sp_type) const;
};

} // namespace layout
} // namespace cbag

#endif
