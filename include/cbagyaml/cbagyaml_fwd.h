/** \file cbag.h
 *  \brief This is the top level header file for cbag library.
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#ifndef CBAGYAML_FWD_H
#define CBAGYAML_FWD_H

#include <cbag/layout/tech.h>
#include <cbag/netlist/netlist_map_t.h>
#include <cbag/schematic/cellview.h>

namespace cbag {

void to_file(const sch::cellview &cv, const char *fname);

sch::cellview cv_from_file(const std::string &yaml_fname, const std::string &sym_view = "");

netlist_map_t read_netlist_map(const char *fname);

} // namespace cbag
#endif
