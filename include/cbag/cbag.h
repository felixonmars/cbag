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

#include <cbag/database/cellviews.h>
#include <cbag/database/datatypes.h>
#include <cbag/database/figures.h>
#include <cbag/database/primitives.h>
#include <cbag/database/shapes.h>
#include <cbag/netlist/name_convert.h>

namespace cbag {
void init_logging();

void to_file(const SchCellView &cv, const char *fname);

void write_netlist(const std::vector<SchCellView *> &cv_list,
                   const std::vector<std::string> &name_list,
                   const char *cell_map,
                   const std::vector<std::string> &inc_list, const char *format,
                   bool flat, bool shell, const char *fname);

} // namespace cbag
#endif // CBAG_CBAG_H
