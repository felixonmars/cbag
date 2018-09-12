
#include <cbag/layout/tech.h>

namespace cbag {
namespace layout {

tech::tech(lay_map_t lay_map, purp_map_t purp_map, via_map_t via_map,
           const std::string &def_purpose, const std::string &pin_purpose_name)
    : lay_map(std::move(lay_map)), purp_map(std::move(purp_map)), via_map(std::move(via_map)),
      default_purpose(purp_map.at(def_purpose)), pin_purpose(purp_map.at(pin_purpose_name)) {}

lay_t tech::get_layer_id(const char *layer) const { return lay_map.at(layer); }

purp_t tech::get_purpose_id(const char *purpose) const {
    return (purpose == nullptr) ? default_purpose : purp_map.at(purpose);
}

lay_t tech::get_bot_layer_id(const char *via) const { return via_map.at(via).first; }

lay_t tech::get_top_layer_id(const char *via) const { return via_map.at(via).second; }

} // namespace layout
} // namespace cbag
