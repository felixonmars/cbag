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

void write_cv_ports(verilog_stream &stream, lstream &b, std::vector<lstream> &line_vec,
                    const std::vector<std::string> &terms, bool &has_prev_term,
                    const char *prefix) {
    for (const auto &term : terms) {
        if (has_prev_term) {
            b.append_last(",");
        } else {
            has_prev_term = true;
        }
        spirit::ast::name_unit ast = util::parse_cdba_name_unit(term);
        b << ast.base;

        line_vec.emplace_back(stream.make_lstream());
        if (ast.is_vector()) {
            (line_vec.back() << prefix << ast.idx_range.to_string(stream.ns, true) << ast.base)
                .append_last(";");
        } else {
            (line_vec.back() << prefix << ast.base).append_last(";");
        }
    }
}

void traits::nstream<verilog_stream>::write_cv_header(type &stream, const std::string &name,
                                                      const sch::cellview_info &info) {
    stream.out_file << std::endl;

    // write module declaration
    bool has_prev_term = false;
    lstream b = stream.make_lstream();
    b << "module" << name << "(";

    std::vector<lstream> line_vec;
    write_cv_ports(stream, b, line_vec, info.in_terms, has_prev_term, "    input");
    write_cv_ports(stream, b, line_vec, info.out_terms, has_prev_term, "    output");
    write_cv_ports(stream, b, line_vec, info.io_terms, has_prev_term, "    inout");
    b << ");";

    stream.out_file << b << std::endl;

    // write port declarations
    for (const auto &line : line_vec) {
        stream.out_file << line;
    }

    stream.out_file << std::endl;
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
