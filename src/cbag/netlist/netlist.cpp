#include <fstream>
#include <memory>

#include <cbag/logging/logging.h>

#include <cbag/netlist/cdl.h>
#include <cbag/netlist/netlist.h>
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

} // namespace cbag
