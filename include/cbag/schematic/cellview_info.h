/** \file cellviews.h
 *  \brief This file defines classes representing various cell views.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SCHEMATIC_CELLVIEW_INFO_H
#define CBAG_SCHEMATIC_CELLVIEW_INFO_H

#include <string>
#include <vector>

#include <cbag/common/datatypes.h>

namespace cbag {
namespace sch {

/** A simple struct representing netlist information of a cellview.
 */
struct cellview_info {
  public:
    std::string cell_name;
    std::vector<std::string> in_terms;
    std::vector<std::string> out_terms;
    std::vector<std::string> io_terms;
    param_map props;
    bool is_prim = false;

    cellview_info();

    cellview_info(std::string name, size_t num_in, size_t num_out, size_t num_inout, bool is_prim);
};

} // namespace sch
} // namespace cbag

#endif
