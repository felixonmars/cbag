
#ifndef CBAG_COMMON_LP_LOOKUP_H
#define CBAG_COMMON_LP_LOOKUP_H

#include <unordered_map>

#include <cbag/common/typedefs.h>

namespace YAML {
class Node;
}

namespace cbag {
namespace layout {

using lay_map_t = std::unordered_map<std::string, lay_t>;
using purp_map_t = std::unordered_map<std::string, purp_t>;

class lp_lookup {
  private:
    purp_t default_purpose;
    purp_t pin_purpose;
    lay_map_t lay_map;
    purp_map_t purp_map;

  public:
    lp_lookup();

    lp_lookup(const YAML::Node &parent);

    purp_t get_default_purpose() const;

    purp_t get_pin_purpose() const;

    std::string get_purpose_name(purp_t purp_id) const;

    std::optional<lay_t> get_layer_id(const std::string &layer) const;

    std::optional<purp_t> get_purpose_id(const std::string &purpose) const;
};

} // namespace layout
} // namespace cbag

#endif
