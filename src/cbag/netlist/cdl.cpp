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

cdl_stream::cdl_stream(const std::string &fname) : nstream_file(fname) {}

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

void append_name_unit(lstream &b, const std::vector<std::string> &names) {
    auto info = spirit::ast::get_ns_info(spirit::ast::namespace_type::CDL);
    for (auto const &name : names) {
        spirit::ast::name_unit ast = cbag::util::parse_cdba_name_unit(name);
        auto stop = ast.end(&info);
        for (auto iter = ast.begin(&info); iter != stop; ++iter) {
            b << *iter;
        }
    }
}
void traits::nstream<cdl_stream>::write_cv_header(type &stream, const std::string &name,
                                                  const sch::cellview_info &info) {
    lstream b = cdl_stream::make_lstream();
    b << ".SUBCKT";
    b << name;
    append_name_unit(b, info.in_terms);
    append_name_unit(b, info.out_terms);
    append_name_unit(b, info.io_terms);

    stream.out_file << b;
}

void traits::nstream<cdl_stream>::write_cv_end(type &stream, const std::string &name) {
    stream.out_file << ".ENDS" << std::endl << std::endl;
}

void append_nets(lstream &b, const std::string &inst_name, const sch::instance &inst,
                 const std::vector<std::string> &terms) {
    for (auto const &term : terms) {
        auto term_iter = inst.connections.find(term);
        if (term_iter == inst.connections.end()) {
            throw std::invalid_argument(fmt::format(
                "Cannot find net connected to instance {} terminal {}", inst_name, term));
        }
        b << term_iter->second;
    }
}

void traits::nstream<cdl_stream>::write_instance(type &stream, const std::string &name,
                                                 const sch::instance &inst,
                                                 const sch::cellview_info &info) {
    lstream b = cdl_stream::make_lstream();

    // <name> <net1> <net2> ... <cell name> <par1>=<val1> ...
    // write instance name
    // TODO: handle array instance correctly.
    b << name;

    // write instance connections
    append_nets(b, name, inst, info.in_terms);
    append_nets(b, name, inst, info.out_terms);
    append_nets(b, name, inst, info.io_terms);

    // write instance cell name
    if (!info.is_prim) {
        b << "/";
    }
    b << inst.cell_name;

    // get default parameter values
    param_map par_map(info.props);
    // update with instance parameters
    for (auto const &pair : inst.params) {
        par_map.insert_or_assign(pair.first, pair.second);
    }
    // write instance parameters
    for (auto const &pair : par_map) {
        std::visit(write_param_visitor(&b, pair.first), pair.second);
    }

    stream.out_file << b;
}

} // namespace netlist
} // namespace cbag
