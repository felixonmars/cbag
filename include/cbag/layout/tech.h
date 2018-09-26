#ifndef CBAG_LAYOUT_TECH_H
#define CBAG_LAYOUT_TECH_H

#include <string>
#include <unordered_map>
#include <utility>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

using lay_map_t = std::unordered_map<std::string, lay_t>;
using purp_map_t = std::unordered_map<std::string, purp_t>;
using via_map_t = std::unordered_map<std::string, std::pair<lay_t, lay_t>>;

class tech {
  private:
    lay_map_t lay_map;
    purp_map_t purp_map;
    via_map_t via_map;
    purp_t default_purpose;

  public:
    purp_t pin_purpose;
    bool make_pin_obj = true;

    tech(lay_map_t lay_map, purp_map_t purp_map, via_map_t via_map, const std::string &def_purpose,
         const std::string &pin_purpose_name, bool make_pin_obj);

    lay_t get_layer_id(const char *layer) const;

    purp_t get_purpose_id(const char *purpose) const;

    void get_via_layers(const std::string &key, lay_t &bot, lay_t &top) const;
};

} // namespace layout
} // namespace cbag

#endif
