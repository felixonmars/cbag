
#ifndef CBAG_COMMON_VIA_LOOKUP_H
#define CBAG_COMMON_VIA_LOOKUP_H

#include <unordered_map>

#include <boost/functional/hash.hpp>

#include <cbag/common/box_t.h>
#include <cbag/common/layer_t.h>
#include <cbag/common/typedefs.h>
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
using vlayers_t = std::pair<layer_t, layer_t>;
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

    via_param get_via_param(vector dim, layer_t bot_layer, layer_t top_layer, orient_2d bot_dir,
                            orient_2d top_dir, bool extend) const;
};

} // namespace layout
} // namespace cbag

#endif
