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
        b << prefix << "wire";
        if (ast.is_vector()) {
            b << ast.idx_range.to_string(stream.ns, true);
        }
        b << ast.base;

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
    write_cv_ports(stream, info.in_terms, has_prev_term, "    input ");
    write_cv_ports(stream, info.out_terms, has_prev_term, "    output");
    write_cv_ports(stream, info.io_terms, has_prev_term, "    inout ");
    if (has_prev_term)
        stream.out_file << std::endl;
    stream.out_file << ");" << std::endl;
}

void traits::nstream<verilog_stream>::write_cv_end(type &stream, const std::string &name) {

    stream.out_file << std::endl << "endmodule" << std::endl;
}

void append_inst_nets(const spirit::namespace_info &ns, verilog_stream &stream,
                      const std::string &inst_name, const sch::instance &inst,
                      const std::vector<std::string> &terms, bool &has_prev_term) {
    for (auto const &term : terms) {
        auto term_iter = inst.connections.find(term);
        if (term_iter == inst.connections.end()) {
            throw std::invalid_argument(fmt::format(
                "Cannot find net connected to instance {} terminal {}", inst_name, term));
        }
        spirit::ast::name_unit term_ast = cbag::util::parse_cdba_name_unit(term);
        lstream b = stream.make_lstream();
        b << "    .";
        b.append_last(term_ast.base).append_last("(");
        b << term_iter->second << ")";

        if (has_prev_term)
            stream.out_file << "," << std::endl;
        else
            has_prev_term = true;
        stream.out_file << b;
    }
}

void traits::nstream<verilog_stream>::write_instance(type &stream, const std::string &name,
                                                     const sch::instance &inst,
                                                     const sch::cellview_info &info) {
    spirit::ast::name_unit inst_ast = cbag::util::parse_cdba_name_unit(name);
    uint32_t n = inst_ast.size();

    if (n == 1) {
        // normal instance, just write normally
        stream.out_file << std::endl;
        lstream b = stream.make_lstream();
        b << inst.cell_name << name << "(";
        stream.out_file << b;

        bool has_prev_term = false;
        append_inst_nets(stream.ns, stream, name, inst, info.in_terms, has_prev_term);
        append_inst_nets(stream.ns, stream, name, inst, info.out_terms, has_prev_term);
        append_inst_nets(stream.ns, stream, name, inst, info.io_terms, has_prev_term);
        if (has_prev_term)
            stream.out_file << std::endl;
        stream.out_file << ");" << std::endl;
    } else {
        // TODO: implement this
    }
}

} // namespace netlist
} // namespace cbag
