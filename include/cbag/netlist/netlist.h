
#ifndef CBAG_NETLIST_NETLIST_H
#define CBAG_NETLIST_NETLIST_H

#include <sstream>
#include <string>
#include <vector>

#include <cbag/enum/design_output.h>
#include <cbag/logging/logging.h>
#include <cbag/netlist/cdl.h>
#include <cbag/netlist/spectre.h>
#include <cbag/netlist/core.h>
#include <cbag/netlist/verilog.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/util/io.h>

namespace cbag {
namespace netlist {

template <class N> using IsNetlister = typename traits::nstream<N>::type;

void read_prim_info(const std::string &prim_fname, std::vector<std::string> &inc_list,
                    sch::netlist_map_t &netlist_map, std::string &append_file,
                    design_output out_type);

template <class ContentList, class N, typename = IsNetlister<N>>
void write_netlist_helper(const ContentList &name_cv_list, N &&stream, bool flat, bool shell,
                          bool top_subckt, sch::netlist_map_t &netlist_map,
                          const std::vector<std::string> &inc_list, const std::string &append_file,
                          spdlog::logger &logger) {

    traits::nstream<N>::write_header(stream, inc_list, shell);

    if (!shell && !append_file.empty()) {
        if (util::is_file(append_file)) {
            std::ifstream text(append_file);
            std::stringstream buffer;
            buffer << text.rdbuf();
            traits::nstream<N>::append_netlist(stream, buffer.str());
        } else {
            throw std::invalid_argument(
                fmt::format("Cannot find primitives append file: {}", append_file));
        }
    }

    auto last_idx = name_cv_list.size() - 1;
    auto stop = name_cv_list.end();
    decltype(name_cv_list.size()) idx = 0;
    for (auto iter = name_cv_list.begin(); iter != stop; ++iter, ++idx) {
        auto is_top = (idx == last_idx);
        auto write_subckt = top_subckt || !is_top;
        auto & [ cur_name, cv_netlist_pair ] = *iter;
        auto & [ cv_ptr, cur_netlist ] = cv_netlist_pair;
        if (cv_ptr) {
            logger.info("Netlisting cellview: {}__{}", cv_ptr->lib_name, cur_name);
            // NOTE: we don't have to compute net attributes if a custom netlist is given
            auto cv_info = sch::get_cv_netlist_info(*cv_ptr, netlist_map, cur_netlist.empty());

            if (!shell || is_top) {
                if (cur_netlist.empty()) {
                    // add this cellview to netlist
                    add_cellview(stream, cur_name, *cv_ptr, cv_info, netlist_map, shell,
                                 write_subckt);
                } else {
                    add_cellview(stream, cur_netlist);
                }
            }

            // add this cellview to netlist map
            logger.info("Adding cellview {}__{} to netlist cell map", cv_ptr->lib_name,
                        cv_ptr->cell_name);
            sch::record_cv_info(netlist_map, std::move(cv_info));
        }
    }

    // build final netlist
    traits::nstream<N>::write_end(stream);
    traits::nstream<N>::close(stream);
}

template <class ContentList>
void write_netlist(const ContentList &name_cv_list, const std::string &fname, design_output format,
                   sch::netlist_map_t &netlist_map, const std::string &append_file,
                   const std::vector<std::string> &inc_list, bool flat = true, bool shell = false,
                   bool top_subckt = true, cnt_t rmin = 2000) {
    auto logger = cbag::get_cbag_logger();

    switch (format) {
    case design_output::CDL:
        logger->info("Writing CDL netlist: {}", fname);
        write_netlist_helper(name_cv_list, cdl_stream(fname, rmin), flat, shell, top_subckt,
                             netlist_map, inc_list, append_file, *logger);
        break;
    case design_output::VERILOG:
        logger->info("Writing Verilog netlist: {}", fname);
        write_netlist_helper(name_cv_list, verilog_stream(fname), flat, shell, top_subckt,
                             netlist_map, inc_list, append_file, *logger);
        break;
    case design_output::SYSVERILOG:
        logger->info("Writing System Verilog netlist: {}", fname);
        write_netlist_helper(name_cv_list, verilog_stream(fname), flat, shell, top_subckt,
                             netlist_map, inc_list, append_file, *logger);
        break;
    case design_output::SPECTRE:
        logger->info("Writing Spectre netlist: {}", fname);
        write_netlist_helper(name_cv_list, spectre_stream(fname, rmin), flat, shell, top_subckt,
                             netlist_map, inc_list, append_file, *logger);
        break;
    default:
        throw std::invalid_argument(
            fmt::format("Unrecognized design output code: {}", static_cast<enum_t>(format)));
    }
}

} // namespace netlist
} // namespace cbag
#endif // CBAG_CBAG_FWD_H
