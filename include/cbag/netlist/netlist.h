
#ifndef CBAG_NETLIST_NETLIST_H
#define CBAG_NETLIST_NETLIST_H

#include <string>
#include <vector>

#include <cbag/netlist/netlist_map_t.h>
#include <cbag/schematic/cellview.h>

namespace cbag {

enum class netlist_fmt : uint8_t {
    CDL = 0,
    VERILOG = 1,
};

void write_netlist(const std::vector<sch::cellview *> &cv_list,
                   const std::vector<std::string> &name_list,
                   const std::vector<std::string> &inc_list, netlist_map_t &netlist_map, bool flat,
                   bool shell, netlist_fmt format, const std::string &fname);

} // namespace cbag
#endif // CBAG_CBAG_FWD_H
