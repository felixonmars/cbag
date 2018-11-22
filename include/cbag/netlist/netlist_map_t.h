/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_NETLIST_NETLIST_MAP_T_H
#define CBAG_NETLIST_NETLIST_MAP_T_H

#include <string>
#include <unordered_map>

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>

namespace cbag {

namespace sch {
struct cellview_info;
}

namespace netlist {

using lib_map_t = std::unordered_map<std::string, sch::cellview_info>;
using netlist_map_t = std::unordered_map<std::string, lib_map_t>;

} // namespace netlist
} // namespace cbag

#endif
