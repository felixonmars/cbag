/** \file cellviews.h
 *  \brief This file defines classes representing various cell views.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SCHEMATIC_CELLVIEW_INFO_H
#define CBAG_SCHEMATIC_CELLVIEW_INFO_H

#include <array>
#include <string>
#include <vector>

#include <cbag/common/param_map.h>
#include <cbag/util/sorted_map.h>

namespace cbag {
namespace sch {

struct cellview;

using attr_map_t = util::sorted_map<std::string, std::string>;

/** A simple struct representing netlist information of a cellview.
 */
struct cellview_info {
  public:
    std::string lib_name;
    std::string cell_name;
    std::vector<std::string> in_terms;
    std::vector<std::string> out_terms;
    std::vector<std::string> io_terms;
    std::vector<std::string> nets;
    param_map props;
    bool is_prim = false;
    util::sorted_map<std::string, attr_map_t> term_net_attrs;

    cellview_info();

    cellview_info(std::string lib_name, std::string cell_name, bool is_prim);
};

using lib_map_t = std::unordered_map<std::string, sch::cellview_info>;
using netlist_map_t = std::unordered_map<std::string, lib_map_t>;

const cellview_info &get_cv_info(const netlist_map_t &info_map, const std::string &lib_name,
                                 const std::string &cell_name);

void record_cv_info(netlist_map_t &info_map, const std::string &lib_name,
                    const std::string &cell_name, cellview_info &&info);

cellview_info get_cv_netlist_info(const cellview &cv, const std::string &cell_name,
                                  const netlist_map_t &info_map, bool compute_net_attrs);

} // namespace sch
} // namespace cbag

#endif
