
#include <fmt/core.h>

#include "yaml-cpp/unordered_map.h"
#include <yaml-cpp/yaml.h>

#include <cbag/enum/design_output.h>
#include <cbag/netlist/netlist.h>
#include <cbag/util/io.h>
#include <cbag/yaml/cellviews.h>
#include <cbag/yaml/enum.h>

namespace cbag {
namespace netlist {

void read_prim_info(const std::string &prim_fname, std::vector<std::string> &inc_list,
                    netlist_map_t &netlist_map, std::string &append_file, design_output out_type) {
    if (prim_fname.empty())
        return;

    if (!util::is_file(prim_fname)) {
        throw std::invalid_argument(
            fmt::format("{} does not point to a netlist primitive file.", prim_fname));
    }

    YAML::Node n = YAML::LoadFile(prim_fname);
    netlist_map = n["netlist_map"].as<netlist_map_t>();
    auto inc_list_map =
        n["inc_list"].as<std::unordered_map<design_output, std::vector<std::string>>>();
    auto iter = inc_list_map.find(out_type);
    if (iter == inc_list_map.end()) {
        throw std::out_of_range("Cannot find include files for netlist output code " +
                                std::to_string(static_cast<enum_t>(out_type)));
    }
    inc_list = iter->second;

    auto app_file_map = n["prim_files"].as<std::unordered_map<design_output, std::string>>();
    auto iter2 = app_file_map.find(out_type);
    if (iter2 == app_file_map.end()) {
        throw std::out_of_range("Cannot find append primitive file for netlist output code " +
                                std::to_string(static_cast<enum_t>(out_type)));
    }
    append_file = iter2->second;
}

} // namespace netlist
} // namespace cbag
