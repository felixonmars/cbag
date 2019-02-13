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

namespace cbag {
namespace sch {

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

    cellview_info();

    cellview_info(std::string name, bool is_prim);
};

} // namespace sch
} // namespace cbag

#endif
