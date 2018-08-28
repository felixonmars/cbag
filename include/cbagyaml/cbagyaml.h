/** \file cbag.h
 *  \brief This is the top level header file for cbag library.
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#ifndef CBAGYAML_H
#define CBAGYAML_H

#include <memory>
#include <string>
#include <vector>

#include <cbag/netlist/netlist.h>

#include <cbagyaml/cellviews.h>

namespace cbag {

void to_file(const sch::cellview &cv, const char *fname);

std::unique_ptr<sch::cellview> from_file(const char *yaml_fname, const char *sym_view);

netlist_map_t read_netlist_map(const char *fname);

} // namespace cbag
#endif
