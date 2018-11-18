#include <fstream>
#include <memory>

#include <cbag/logging/logging.h>

#include <cbag/cbag_fwd.h>
#include <cbag/netlist/cdl.h>
#include <cbag/netlist/verilog.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>

namespace cbag {

std::unique_ptr<netlist_builder> make_netlist_builder(const std::string &fname, netlist_fmt format,
                                                      spdlog::logger &logger) {
    uint8_t fmt_code = static_cast<uint8_t>(format);
    logger.info("Creating netlist builder for netlist format code: {}", fmt_code);

    switch (format) {
    case netlist_fmt::CDL:
        return std::make_unique<cdl_builder>(fname);
    case netlist_fmt::VERILOG:
        return std::make_unique<verilog_builder>(fname);
    default:
        throw std::invalid_argument(fmt::format("Unrecognized netlist format code: {}", fmt_code));
    }
}

void write_netlist(const std::vector<sch::cellview *> &cv_list,
                   const std::vector<std::string> &name_list,
                   const std::vector<std::string> &inc_list, netlist_map_t &netlist_map, bool flat,
                   bool shell, netlist_fmt format, const std::string &fname) {
    auto logger = cbag::get_cbag_logger();
    logger->info("Writing netlist file: {}", fname);

    auto builder_ptr = make_netlist_builder(fname, format, *logger);
    builder_ptr->init(inc_list, shell);

    size_t num = cv_list.size();
    for (size_t idx = 0; idx < num; ++idx) {
        if (!shell || idx == num - 1) {
            // add this cellview to netlist
            logger->info("Netlisting cellview: {}", name_list[idx]);
            sch::cellview_info cv_info = cv_list[idx]->get_info(name_list[idx]);
            builder_ptr->add_cellview(name_list[idx], cv_list[idx], cv_info, netlist_map, shell);
            // add this cellview to netlist map
            logger->info("Adding cellview to netlist cell map");
            auto lib_map_iter = netlist_map.find(cv_list[idx]->lib_name);
            if (lib_map_iter == netlist_map.end()) {
                logger->info("Cannot find library {}, creating lib cell map",
                             cv_list[idx]->lib_name);
                lib_map_t new_lib_map;
                new_lib_map.emplace(cv_list[idx]->cell_name, cv_info);
                netlist_map.emplace(cv_list[idx]->lib_name, std::move(new_lib_map));
            } else {
                lib_map_iter->second.emplace(cv_list[idx]->cell_name, cv_info);
            }
        }
    }

    // build final netlist
    builder_ptr->build();
}

} // namespace cbag
