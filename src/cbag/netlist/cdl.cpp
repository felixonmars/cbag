/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <variant>

#include <fmt/core.h>

#include <cbag/spirit/ast.h>

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/schematic/instance.h>

#include <cbag/netlist/cdl.h>
#include <cbag/util/name_convert.h>

namespace cbag {
namespace netlist {

cdl_stream::cdl_stream(const std::string &fname)
    : nstream_file(fname, spirit::namespace_type::CDL) {}

lstream cdl_stream::make_lstream() { return {ncol, cnt_char, break_before, tab_size}; }

void traits::nstream<cdl_stream>::close(type &stream) { stream.close(); }

void traits::nstream<cdl_stream>::write_header(type &stream,
                                               const std::vector<std::string> &inc_list,
                                               bool shell) {
    if (!shell) {
        if (!inc_list.empty()) {
            for (auto const &fname : inc_list) {
                stream.out_file << ".INCLUDE " << fname << std::endl;
            }
            stream.out_file << std::endl;
        }
        stream.out_file << ".PARAM" << std::endl;
        stream.out_file << std::endl;
    }
}

void traits::nstream<cdl_stream>::write_end(type &stream) {}

void append_name_unit(const spirit::namespace_info &ns, lstream &b,
                      const std::vector<std::string> &names) {
    for (auto const &name : names) {
        spirit::ast::name_unit ast = cbag::util::parse_cdba_name_unit(name);
        ast.append_name_bits(ns, b.get_back_inserter());
    }
}
void traits::nstream<cdl_stream>::write_cv_header(type &stream, const std::string &name,
                                                  const sch::cellview_info &info) {
    lstream b = cdl_stream::make_lstream();
    b << ".SUBCKT";
    b << name;
    append_name_unit(stream.ns, b, info.in_terms);
    append_name_unit(stream.ns, b, info.out_terms);
    append_name_unit(stream.ns, b, info.io_terms);

    stream.out_file << b;
}

void traits::nstream<cdl_stream>::write_cv_end(type &stream, const std::string &name) {
    stream.out_file << ".ENDS" << std::endl << std::endl;
}

void append_nets(const spirit::namespace_info &ns, lstream &b, const std::string &inst_name,
                 const sch::instance &inst, const std::vector<std::string> &terms) {
    for (auto const &term : terms) {
        auto term_iter = inst.connections.find(term);
        if (term_iter == inst.connections.end()) {
            throw std::invalid_argument(fmt::format(
                "Cannot find net connected to instance {} terminal {}", inst_name, term));
        }
        spirit::ast::name ast = cbag::util::parse_cdba_name(term_iter->second);
        ast.append_name_bits(ns, b.get_back_inserter());
    }
}

template <class OutIter>
void write_instance_cell_name(OutIter &&iter, const sch::instance &inst,
                              const sch::cellview_info &info) {
    if (!info.is_prim) {
        *iter = "/";
    }
    *iter = inst.cell_name;

    // get default parameter values
    param_map par_map(info.props);
    // update with instance parameters
    for (auto const &pair : inst.params) {
        par_map.insert_or_assign(pair.first, pair.second);
    }
    // write instance parameters
    for (auto const &pair : par_map) {
        std::visit(write_param_visitor(iter, pair.first), pair.second);
    }
}

void traits::nstream<cdl_stream>::write_instance(type &stream, const std::string &name,
                                                 const sch::instance &inst,
                                                 const sch::cellview_info &info) {
    spirit::ast::name_unit inst_ast = cbag::util::parse_cdba_name_unit(name);
    uint32_t n = inst_ast.size();

    // if (n == 1) {
    // normal instance, just write normally
    lstream b = cdl_stream::make_lstream();
    b << name;
    append_nets(stream.ns, b, name, inst, info.in_terms);
    append_nets(stream.ns, b, name, inst, info.out_terms);
    append_nets(stream.ns, b, name, inst, info.io_terms);
    cbag::netlist::write_instance_cell_name(b.get_back_inserter(), inst, info);
    stream.out_file << b;
    /*
} else {
    // arrayed instance, need to split up
    // first get cell name tokens
    std::vector<std::string> tokens;
    tokens.reserve(2);
    cbag::netlist::write_instance_cell_name(std::back_inserter(tokens), inst, info);
    // array instance
    for (uint32_t inst_idx = 0; inst_idx < n; ++inst_idx) {
        lstream b = cdl_stream::make_lstream();
    }
}
    */
}

} // namespace netlist
} // namespace cbag
