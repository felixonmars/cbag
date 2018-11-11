/** \file cbag.h
 *  \brief This is the top level header file for cbag library.
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#ifndef CBAG_CBAG_FWD_H
#define CBAG_CBAG_FWD_H

#include <string>
#include <vector>

#include <cbag/netlist/netlist_map_t.h>
#include <cbag/schematic/cellview.h>

namespace cbag {

void write_netlist(const std::vector<sch::cellview *> &cv_list,
                   const std::vector<std::string> &name_list,
                   const std::vector<std::string> &inc_list, netlist_map_t &netlist_map,
                   const char *format, bool flat, bool shell, const char *fname);

} // namespace cbag
#endif // CBAG_CBAG_FWD_H
