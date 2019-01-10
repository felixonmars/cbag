
#ifndef CBAG_COMMON_VIA_LOOKUP_H
#define CBAG_COMMON_VIA_LOOKUP_H

#include <array>
#include <unordered_map>

#include <boost/container_hash/hash.hpp>

#include <cbag/common/box_t.h>
#include <cbag/common/layer_t.h>
#include <cbag/common/typedefs.h>
#include <cbag/enum/direction.h>
#include <cbag/enum/orient_2d.h>
#include <cbag/layout/via_info.h>
#include <cbag/layout/via_param.h>

namespace YAML {
class Node;
}

namespace cbag {
namespace layout {

class lp_lookup;

using via_lay_purp_t = std::tuple<layer_t, layer_t, layer_t>;
using vlp_map_t = std::unordered_map<std::string, via_lay_purp_t>;
using vlayers_t = std::array<layer_t, 2>;
using vid_map_t = std::unordered_map<vlayers_t, std::string, boost::hash<vlayers_t>>;
using vinfo_map_t = std::unordered_map<std::string, std::vector<via_info>>;

class via_lookup {
  private:
    vlp_map_t lp_map;
    vid_map_t id_map;
    vinfo_map_t info_map;

  public:
    via_lookup();

    via_lookup(const YAML::Node &parent, const lp_lookup &lp);

    via_lay_purp_t get_via_layer_purpose(const std::string &key) const;

    const std::string &get_via_id(direction vdir, layer_t layer, layer_t adj_layer) const;

    via_param get_via_param(vector dim, const std::string &via_id, direction vdir, orient_2d ex_dir,
                            orient_2d adj_ex_dir, bool extend) const;
};

} // namespace layout
} // namespace cbag

#endif
