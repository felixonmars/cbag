
#ifndef CBAG_NETLIST_NETLIST_H
#define CBAG_NETLIST_NETLIST_H

#include <string>
#include <vector>

#include <cbag/logging/logging.h>
#include <cbag/netlist/cdl.h>
#include <cbag/netlist/core.h>
#include <cbag/netlist/netlist_map_t.h>
#include <cbag/netlist/verilog.h>

namespace cbag {
namespace netlist {

enum class netlist_fmt : uint8_t {
    CDL = 0,
    VERILOG = 1,
};

template <class N> using IsNetlister = typename traits::nstream<N>::type;

template <class ContentList>
void write_netlist(const ContentList &name_cv_list, const std::string &fname, netlist_fmt format,
                   bool flat = true, bool shell = false, uint32_t rmin = 2000,
                   const std::string &prim_fname = "") {
    auto logger = cbag::get_cbag_logger();

    switch (format) {
    case netlist_fmt::CDL:
        logger->info("Writing CDL netlist: {}", fname);
        write_netlist(name_cv_list, cdl_stream(fname, rmin), flat, shell, prim_fname, *logger);
        break;
    case netlist_fmt::VERILOG:
        logger->info("Writing Verilog netlist: {}", fname);
        write_netlist(name_cv_list, verilog_stream(fname), flat, shell, prim_fname, *logger);
        break;
    default:
        throw std::invalid_argument(
            fmt::format("Unrecognized netlist format code: {}", static_cast<uint8_t>(format)));
    }
}

void read_prim_info(const std::string &prim_fname, std::vector<std::string> &inc_list,
                    netlist_map_t &netlist_map);

template <class ContentList, class N, typename = IsNetlister<N>>
void write_netlist(const ContentList &name_cv_list, N &&stream, bool flat, bool shell,
                   const std::string &prim_fname, spdlog::logger &logger) {
    std::vector<std::string> inc_list;
    netlist_map_t netlist_map;
    read_prim_info(prim_fname, inc_list, netlist_map);

    traits::nstream<N>::write_header(stream, inc_list, shell);

    std::size_t num = name_cv_list.size();
    auto iter = name_cv_list.begin();
    for (std::size_t idx = 0; idx < num; ++idx, ++iter) {
        if (!shell || idx == num - 1) {
            auto &cur_pair = *iter;
            auto &cur_cv = cur_pair.second;
            const std::string &cur_name = cur_pair.first;
            // add this cellview to netlist
            logger.info("Netlisting cellview: {}", cur_name);
            sch::cellview_info cv_info = cur_cv->get_info(cur_name);
            add_cellview(stream, cur_name, *cur_cv, cv_info, netlist_map, shell);
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

} // namespace netlist
} // namespace cbag
#endif // CBAG_CBAG_FWD_H
