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

verilog_stream::verilog_stream(const std::string &fname)
    : nstream_file(fname, spirit::namespace_type::VERILOG) {}

lstream verilog_stream::make_lstream() { return {ncol, "", break_before, tab_size}; }

void traits::nstream<verilog_stream>::close(type &stream) { stream.close(); }

void traits::nstream<verilog_stream>::write_header(type &stream,
                                                   const std::vector<std::string> &inc_list,
                                                   bool shell) {
    // TODO: Add actual implementation
}

void traits::nstream<verilog_stream>::write_end(type &stream) {}

void write_cv_ports(lstream &b, const std::vector<std::string> &terms, bool &has_prev_term) {
    for (const auto &term : terms) {
        if (has_prev_term) {
            b.append_last(",");
        } else {
            has_prev_term = true;
        }
        spirit::ast::name_unit ast = util::parse_cdba_name_unit(term);
        b << ast.base;
    }
}

void traits::nstream<verilog_stream>::write_cv_header(type &stream, const std::string &name,
                                                      const sch::cellview_info &info) {
    // write module declaration
    bool has_prev_term = false;
    lstream b = verilog_stream::make_lstream();
    b << "module" << name << "(";
    write_cv_ports(b, info.in_terms, has_prev_term);
    write_cv_ports(b, info.out_terms, has_prev_term);
    write_cv_ports(b, info.io_terms, has_prev_term);
    b << ");";

    stream.out_file << b << std::endl;

    // write io type
    for (auto const &name : info.in_terms) {
        stream.out_file << "    input " << name << ";" << std::endl;
    }
    for (auto const &name : info.out_terms) {
        stream.out_file << "    output " << name << ";" << std::endl;
    }
    for (auto const &name : info.io_terms) {
        stream.out_file << "    inout " << name << ";" << std::endl;
    }
    stream.out_file << std::endl;
}

void traits::nstream<verilog_stream>::write_cv_end(type &stream, const std::string &name) {
    stream.out_file << "endmodule" << std::endl << std::endl;
}

void traits::nstream<verilog_stream>::write_instance(type &stream, const std::string &name,
                                                     const sch::instance &inst,
                                                     const sch::cellview_info &info) {
    // TODO: add actual implementation
}

} // namespace netlist
} // namespace cbag
