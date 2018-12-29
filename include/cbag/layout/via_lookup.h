
#ifndef CBAG_COMMON_VIA_LOOKUP_H
#define CBAG_COMMON_VIA_LOOKUP_H

#include <unordered_map>

#include <boost/functional/hash.hpp>

#include <cbag/common/layer_t.h>
#include <cbag/common/typedefs.h>
#include <cbag/layout/via_info.h>

namespace YAML {
class Node;
}

namespace cbag {
namespace layout {

using lay_map_t = std::unordered_map<std::string, lay_t>;
using purp_map_t = std::unordered_map<std::string, purp_t>;
using via_lay_purp_t = std::tuple<layer_t, layer_t, layer_t>;
using vlp_map_t = std::unordered_map<std::string, via_lay_purp_t>;
using vname_map_t = std::unordered_map<int_t, std::string>;
using vlayers_t = std::pair<layer_t, layer_t>;
using vid_map_t = std::unordered_map<vlayers_t, std::string, boost::hash<vlayers_t>>;
using vinfo_map_t = std::unordered_map<std::string, std::vector<via_info>>;

class via_lookup {
  private:
    vname_map_t name_map;
    vlp_map_t lp_map;
    vid_map_t id_map;
    vinfo_map_t info_map;

  public:
    via_lookup();

    via_lookup(const YAML::Node &parent, const lay_map_t &lay_map, const purp_map_t &purp_map);

    via_lay_purp_t get_via_layer_purpose(const std::string &key) const;
};

} // namespace layout
} // namespace cbag

#endif
