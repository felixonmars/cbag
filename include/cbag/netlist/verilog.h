/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_NETLIST_VERILOG_H
#define CBAG_NETLIST_VERILOG_H

#include <cbag/netlist/core.h>

namespace cbag {
namespace netlist {

class verilog_stream : public nstream_file {
  public:
    explicit verilog_stream(const std::string &fname);
};

template <> struct traits::nstream<verilog_stream> {
    using type = verilog_stream;

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

#endif // CBAG_NETLIST_VERILOG_H
