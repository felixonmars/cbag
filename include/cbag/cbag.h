/** \file cbag.h
 *  \brief This is the top level header file for cbag library.
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#ifndef CBAG_CBAG_H
#define CBAG_CBAG_H

#include <string>
#include <vector>

#include <cbag/common/datatypes.h>
#include <cbag/common/primitives.h>
#include <cbag/netlist/name_convert.h>
#include <cbag/schematic/cellviews.h>
#include <cbag/schematic/figures.h>
#include <cbag/schematic/shapes.h>

namespace cbag {
void init_logging();

void to_file(const SchCellView &cv, const char *fname);

void write_netlist(const std::vector<SchCellView *> &cv_list,
                   const std::vector<std::string> &name_list, const char *cell_map,
                   const std::vector<std::string> &inc_list, const char *format, bool flat,
                   bool shell, const char *fname);

} // namespace cbag
#endif // CBAG_CBAG_H
