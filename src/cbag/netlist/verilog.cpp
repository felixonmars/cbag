/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <boost/range/join.hpp>

#include <fmt/core.h>

#include <cbag/netlist/verilog.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/schematic/instance.h>
#include <cbag/spirit/ast.h>
#include <cbag/util/name_convert.h>

namespace cbag {
namespace netlist {

line_format get_verilog_line_format() { return {120, "", true, 4}; }

verilog_stream::verilog_stream(const std::string &fname)
    : nstream_file(fname, spirit::namespace_type::VERILOG, get_verilog_line_format()) {}

void traits::nstream<verilog_stream>::close(type &stream) { stream.close(); }

void traits::nstream<verilog_stream>::write_header(type &stream,
                                                   const std::vector<std::string> &inc_list,
                                                   bool shell) {
    // TODO: Add actual implementation
}

void traits::nstream<verilog_stream>::write_end(type &stream) {}

void write_cv_ports(verilog_stream &stream, const std::vector<std::string> &terms,
                    bool &has_prev_term, const char *prefix) {
    for (const auto &term : terms) {
        spirit::ast::name_unit ast = util::parse_cdba_name_unit(term);

        auto b = stream.make_lstream();
        b << prefix;
        if (ast.is_vector()) {
            b << ast.idx_range.to_string(stream.ns, true);
        }
        b << "wire" << ast.base;

        if (has_prev_term)
            stream.out_file << "," << std::endl;
        else
            has_prev_term = true;
        b.append_to(stream.out_file, false);
    }
}

void traits::nstream<verilog_stream>::write_cv_header(type &stream, const std::string &name,
                                                      const sch::cellview_info &info) {
    stream.out_file << std::endl;

    // write module declaration
    lstream b = stream.make_lstream();
    (b << "module" << name).append_last("(");
    stream.out_file << b;

    bool has_prev_term = false;
    write_cv_ports(stream, info.in_terms, has_prev_term, "    input");
    write_cv_ports(stream, info.out_terms, has_prev_term, "    output");
    write_cv_ports(stream, info.io_terms, has_prev_term, "    inout");
    if (has_prev_term)
        stream.out_file << std::endl;
    stream.out_file << ");" << std::endl << std::endl;
}

void traits::nstream<verilog_stream>::write_cv_end(type &stream, const std::string &name) {
    stream.out_file << "endmodule" << std::endl;
}

void traits::nstream<verilog_stream>::write_instance(type &stream, const std::string &name,
                                                     const sch::instance &inst,
                                                     const sch::cellview_info &info) {
    // TODO: add actual implementation
}

} // namespace netlist
} // namespace cbag
