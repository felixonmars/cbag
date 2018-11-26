
#ifndef CBAG_NETLIST_NETLIST_H
#define CBAG_NETLIST_NETLIST_H

#include <string>
#include <vector>

#include <cbag/common/design_output.h>
#include <cbag/logging/logging.h>
#include <cbag/netlist/cdl.h>
#include <cbag/netlist/core.h>
#include <cbag/netlist/netlist_map_t.h>
#include <cbag/netlist/verilog.h>

namespace cbag {
namespace netlist {

template <class N> using IsNetlister = typename traits::nstream<N>::type;

void read_prim_info(const std::string &prim_fname, std::vector<std::string> &inc_list,
                    netlist_map_t &netlist_map, design_output out_type);

template <class ContentList, class N, typename = IsNetlister<N>>
void write_netlist_helper(const ContentList &name_cv_list, N &&stream, bool flat, bool shell,
                          netlist_map_t &netlist_map, const std::vector<std::string> &inc_list,
                          spdlog::logger &logger) {

    traits::nstream<N>::write_header(stream, inc_list, shell);

    auto last_idx = name_cv_list.size() - 1;
    auto stop = name_cv_list.end();
    decltype(name_cv_list.size()) idx = 0;
    for (auto iter = name_cv_list.begin(); iter != stop; ++iter, ++idx) {
        if (!shell || idx == last_idx) {
            auto &cur_pair = *iter;
            const std::string &cur_name = cur_pair.first;
            const auto &cur_cv = cur_pair.second.first;
            const std::string &cur_netlist = cur_pair.second.second;
            sch::cellview_info cv_info = cur_cv->get_info(cur_name);
            logger.info("Netlisting cellview: {}", cur_name);

            if (cur_netlist.empty()) {
                // add this cellview to netlist
                add_cellview(stream, cur_name, *cur_cv, cv_info, netlist_map, shell);
            } else {
                add_cellview(stream, cur_netlist);
            }

            // add this cellview to netlist map
            logger.info("Adding cellview to netlist cell map");
            auto lib_map_iter = netlist_map.find(cur_cv->lib_name);
            if (lib_map_iter == netlist_map.end()) {
                logger.info("Cannot find library {}, creating lib cell map", cur_cv->lib_name);
                lib_map_t new_lib_map;
                new_lib_map.emplace(cur_cv->cell_name, cv_info);
                netlist_map.emplace(cur_cv->lib_name, std::move(new_lib_map));
            } else {
                lib_map_iter->second.emplace(cur_cv->cell_name, cv_info);
            }
        }
    }

    // build final netlist
    traits::nstream<N>::write_end(stream);
    traits::nstream<N>::close(stream);
}

template <class ContentList>
void write_netlist(const ContentList &name_cv_list, const std::string &fname, design_output format,
                   bool flat = true, bool shell = false, uint32_t rmin = 2000,
                   const std::string &prim_fname = "") {
    auto logger = cbag::get_cbag_logger();

    std::vector<std::string> inc_list;
    netlist_map_t netlist_map;
    read_prim_info(prim_fname, inc_list, netlist_map, format);

    switch (format) {
    case design_output::CDL:
        logger->info("Writing CDL netlist: {}", fname);
        write_netlist_helper(name_cv_list, cdl_stream(fname, rmin), flat, shell, netlist_map,
                             inc_list, *logger);
        break;
    case design_output::VERILOG:
        logger->info("Writing Verilog netlist: {}", fname);
        write_netlist_helper(name_cv_list, verilog_stream(fname), flat, shell, netlist_map,
                             inc_list, *logger);
        break;
    default:
        throw std::invalid_argument(
            fmt::format("Unrecognized design output code: {}", static_cast<uint8_t>(format)));
    }
}

} // namespace netlist
} // namespace cbag
#endif // CBAG_CBAG_FWD_H
