
#include <boost/filesystem.hpp>

#include <fmt/core.h>

#include "yaml-cpp/unordered_map.h"
#include <yaml-cpp/yaml.h>

#include <cbag/netlist/netlist.h>
#include <cbag/yaml/cellviews.h>

namespace fs = boost::filesystem;

namespace cbag {
namespace netlist {

void read_prim_info(const std::string &prim_fname, std::vector<std::string> &inc_list,
                    netlist_map_t &netlist_map) {
    if (prim_fname.empty())
        return;

    fs::path fpath(prim_fname);

    if (!fs::is_regular_file(fpath)) {
        throw std::invalid_argument(
            fmt::format("{} does not point to a netlist primitive file.", prim_fname));
    }

    YAML::Node n = YAML::LoadFile(prim_fname);
    inc_list = n["inc_list"].as<std::vector<std::string>>();
    netlist_map = n["netlist_map"].as<netlist_map_t>();
}

} // namespace netlist
} // namespace cbag
