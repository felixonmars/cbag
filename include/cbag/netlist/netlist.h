
#ifndef CBAG_NETLIST_NETLIST_H
#define CBAG_NETLIST_NETLIST_H

#include <memory>
#include <string>
#include <vector>

#include <cbag/logging/logging.h>
#include <cbag/netlist/core.h>
#include <cbag/netlist/netlist_map_t.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>

namespace cbag {
namespace netlist {

enum class netlist_fmt : uint8_t {
    CDL = 0,
    VERILOG = 1,
};

std::unique_ptr<netlist_builder> make_netlist_builder(const std::string &fname, netlist_fmt format,
                                                      spdlog::logger &logger);

template <class VectorType>
void write_netlist(const VectorType &cv_name_list, const std::vector<std::string> &inc_list,
                   netlist_map_t &netlist_map, bool flat, bool shell, netlist_fmt format,
                   const std::string &fname) {
    auto logger = cbag::get_cbag_logger();
    logger->info("Writing netlist file: {}", fname);

    auto builder_ptr = make_netlist_builder(fname, format, *logger);
    builder_ptr->init(inc_list, shell);

    std::size_t num = cv_name_list.size();
    auto iter = cv_name_list.begin();
    for (std::size_t idx = 0; idx < num; ++idx, ++iter) {
        if (!shell || idx == num - 1) {
            auto &cur_pair = *iter;
            auto &cur_cv = cur_pair.first;
            const std::string &cur_name = cur_pair.second;
            // add this cellview to netlist
            logger->info("Netlisting cellview: {}", cur_name);
            sch::cellview_info cv_info = cur_cv->get_info(cur_name);
            builder_ptr->add_cellview(cur_name, *cur_cv, cv_info, netlist_map, shell);
            // add this cellview to netlist map
            logger->info("Adding cellview to netlist cell map");
            auto lib_map_iter = netlist_map.find(cur_cv->lib_name);
            if (lib_map_iter == netlist_map.end()) {
                logger->info("Cannot find library {}, creating lib cell map", cur_cv->lib_name);
                lib_map_t new_lib_map;
                new_lib_map.emplace(cur_cv->cell_name, cv_info);
                netlist_map.emplace(cur_cv->lib_name, std::move(new_lib_map));
            } else {
                lib_map_iter->second.emplace(cur_cv->cell_name, cv_info);
            }
        }
    }

    // build final netlist
    builder_ptr->build();
}

} // namespace netlist
} // namespace cbag
#endif // CBAG_CBAG_FWD_H
