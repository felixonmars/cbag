/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Ayan Biswas
 *  \date   2019/02/04
 */

#ifndef CBAG_NETLIST_SPECTRE_H
#define CBAG_NETLIST_SPECTRE_H

#include <cbag/netlist/core.h>

namespace cbag {
namespace netlist {

class spectre_stream : public nstream_file {
  public:
    cnt_t rmin;
    std::unordered_map <std::string, std::string> cdf2spectre_names;
    std::unordered_map <std::string, std::string> cdf2spectre_props;

    spectre_stream(const std::string &fname, cnt_t rmin);
};

template <> struct traits::nstream<spectre_stream> {
    using type = spectre_stream;

    static void close(type &stream);

    static void write_header(type &stream, const std::vector<std::string> &inc_list, bool shell);

    static void write_end(type &stream);

    static void write_cv_header(type &stream, const std::string &name,
                                const sch::cellview_info &info, bool shell, bool write_subckt);

    static void write_cv_end(type &stream, const std::string &name, bool write_subckt);

    static void write_instance(type &stream, const std::string &name, const sch::instance &inst,
                               const sch::cellview_info &info);

    static void append_netlist(type &stream, const std::string &netlist);
};

} // namespace netlist
} // namespace cbag

#endif // CBAG_NETLIST_SPECTRE_H
