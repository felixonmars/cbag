
#ifndef CBAG_NETLIST_NETLIST_H
#define CBAG_NETLIST_NETLIST_H

#include <memory>
#include <string>
#include <vector>

#include <cbag/logging/logging.h>
#include <cbag/netlist/cdl.h>
#include <cbag/netlist/core.h>
#include <cbag/netlist/netlist_map_t.h>
#include <cbag/netlist/verilog.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>

namespace cbag {
namespace netlist {

enum class netlist_fmt : uint8_t {
    CDL = 0,
    VERILOG = 1,
};

template <class VectorType>
void write_netlist(const VectorType &cv_name_list, const std::vector<std::string> &inc_list,
                   netlist_map_t &netlist_map, bool flat, bool shell, netlist_fmt format,
                   const std::string &fname, uint32_t rmin = 2000) {
    auto logger = cbag::get_cbag_logger();
    logger->info("Writing netlist file: {}", fname);

    uint8_t fmt_code = static_cast<uint8_t>(format);
    logger->info("Creating netlist builder for netlist format code: {}", fmt_code);

    switch (format) {
    case netlist_fmt::CDL:
        write_netlist(cv_name_list, inc_list, netlist_map, flat, shell, cdl_stream(fname, rmin),
                      *logger);
        break;
    case netlist_fmt::VERILOG:
        write_netlist(cv_name_list, inc_list, netlist_map, flat, shell, verilog_stream(fname),
                      *logger);
        break;
    default:
        throw std::invalid_argument(fmt::format("Unrecognized netlist format code: {}", fmt_code));
    }
}

template <class VectorType, class Netlister>
void write_netlist(const VectorType &cv_name_list, const std::vector<std::string> &inc_list,
                   netlist_map_t &netlist_map, bool flat, bool shell, Netlister stream,
                   spdlog::logger &logger) {
    traits::nstream<Netlister>::write_header(stream, inc_list, shell);

    std::size_t num = cv_name_list.size();
    auto iter = cv_name_list.begin();
    for (std::size_t idx = 0; idx < num; ++idx, ++iter) {
        if (!shell || idx == num - 1) {
            auto &cur_pair = *iter;
            auto &cur_cv = cur_pair.first;
            const std::string &cur_name = cur_pair.second;
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
    traits::nstream<Netlister>::write_end(stream);
    traits::nstream<Netlister>::close(stream);
}

} // namespace netlist
} // namespace cbag
#endif // CBAG_CBAG_FWD_H
