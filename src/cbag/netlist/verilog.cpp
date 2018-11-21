/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <boost/range/join.hpp>

#include <fmt/core.h>

#include <cbag/netlist/lstream.h>
#include <cbag/netlist/verilog.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/schematic/instance.h>
#include <cbag/spirit/ast.h>
#include <cbag/util/name_convert.h>

namespace cbag {
namespace netlist {

verilog_stream::verilog_stream(const std::string &fname) : nstream_file(fname) {}

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

        lstream b;
        b << prefix << "wire";
        if (ast.is_vector()) {
            if (ast.idx_range.step != 1) {
                throw std::invalid_argument(
                    "Verilog does not support port with non-unity step size: " + term);
            }
            b << fmt::format("[{}:{}]", ast.idx_range.start, ast.idx_range.get_stop_include());
        }
        b << ast.base;

        if (has_prev_term)
            stream.out_file << "," << std::endl;
        else
            has_prev_term = true;
        b.to_file(stream.out_file, spirit::namespace_verilog{}, false);
    }
}

void traits::nstream<verilog_stream>::write_cv_header(type &stream, const std::string &name,
                                                      const sch::cellview_info &info) {
    stream.out_file << std::endl;

    // write module declaration
    lstream b;
    (b << "module" << name).append_last("(");
    b.to_file(stream.out_file, spirit::namespace_verilog{});

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

void append_inst_nets(verilog_stream &stream, const std::string &inst_name,
                      const sch::instance &inst, const std::vector<std::string> &terms,
                      bool &has_prev_term) {
    for (auto const &term : terms) {
        auto term_iter = inst.connections.find(term);
        if (term_iter == inst.connections.end()) {
            throw std::invalid_argument(fmt::format(
                "Cannot find net connected to instance {} terminal {}", inst_name, term));
        }
        spirit::ast::name_unit term_ast = cbag::util::parse_cdba_name_unit(term);
        lstream b;
        b << "    .";
        b.append_last(term_ast.base).append_last("(");
        spirit::ast::name net_ast = cbag::util::parse_cdba_name(term_iter->second);
        b << net_ast.to_string(spirit::namespace_verilog{}) << ")";

        if (has_prev_term)
            stream.out_file << "," << std::endl;
        else
            has_prev_term = true;
        b.to_file(stream.out_file, spirit::namespace_verilog{});
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
        lstream b;
        b << inst.cell_name << name << "(";
        b.to_file(stream.out_file, spirit::namespace_verilog{});

        bool has_prev_term = false;
        append_inst_nets(stream, name, inst, info.in_terms, has_prev_term);
        append_inst_nets(stream, name, inst, info.out_terms, has_prev_term);
        append_inst_nets(stream, name, inst, info.io_terms, has_prev_term);
        if (has_prev_term)
            stream.out_file << std::endl;
        stream.out_file << ");" << std::endl;
    } else {
        // TODO: implement this
    }
}

} // namespace netlist
} // namespace cbag
