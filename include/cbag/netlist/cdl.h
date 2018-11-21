/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_NETLIST_CDL_H
#define CBAG_NETLIST_CDL_H

#include <cbag/netlist/core.h>

namespace cbag {
namespace netlist {

class cdl_stream : public nstream_file {
  public:
    uint32_t rmin;

    cdl_stream(const std::string &fname, uint32_t rmin);
};

template <> struct traits::nstream<cdl_stream> {
    using type = cdl_stream;

    static void close(type &stream);

    static void write_header(type &stream, const std::vector<std::string> &inc_list, bool shell);

    static void write_end(type &stream);

    static void write_cv_header(type &stream, const std::string &name,
                                const sch::cellview_info &info);

    static void write_cv_end(type &stream, const std::string &name);

    static void write_instance(type &stream, const std::string &name, const sch::instance &inst,
                               const sch::cellview_info &info);
};

} // namespace netlist
} // namespace cbag

#endif // CBAG_NETLIST_CDL_H
