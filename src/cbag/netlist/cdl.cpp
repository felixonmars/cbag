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

cdl_builder::cdl_builder(const std::string &fname) : netlist_builder(fname) {}

void cdl_builder::init(const std::vector<std::string> &inc_list, bool shell) {
    if (!shell) {
        for (auto const &fname : inc_list) {
            out_file << ".INCLUDE " << fname << std::endl;
        }
        out_file << std::endl;
        out_file << ".PARAM" << std::endl;
        out_file << std::endl;
    }
}

void cdl_builder::write_end() {}

void append_name_unit(cdl_builder::line_builder &b, const std::vector<std::string> &names) {
    auto info = spirit::ast::get_ns_info(spirit::ast::namespace_type::CDL);
    for (auto const &name : names) {
        spirit::ast::name_unit ast = cbag::util::parse_cdba_name_unit(name);
        auto stop = ast.end(&info);
        for (auto iter = ast.begin(&info); iter != stop; ++iter) {
            b << *iter;
        }
    }
}

void cdl_builder::write_cv_header(const std::string &name, const sch::cellview_info &info) {
    line_builder b(ncol, cnt_char, break_before, tab_size);
    b << ".SUBCKT";
    b << name;
    append_name_unit(b, info.in_terms);
    append_name_unit(b, info.out_terms);
    append_name_unit(b, info.io_terms);

    out_file << b;
}

void cdl_builder::write_cv_end(const std::string &name) { out_file << ".ENDS" << std::endl; }

void append_nets(cdl_builder::line_builder &b, const std::string &inst_name,
                 const sch::instance &inst, const std::vector<std::string> &terms) {
    for (auto const &term : terms) {
        auto term_iter = inst.connections.find(term);
        if (term_iter == inst.connections.end()) {
            throw std::invalid_argument(fmt::format(
                "Cannot find net connected to instance {} terminal {}", inst_name, term));
        }
        b << term_iter->second;
    }
}

void cdl_builder::write_instance_helper(const std::string &name, const sch::instance &inst,
                                        const sch::cellview_info &info) {
    line_builder b(ncol, cnt_char, break_before, tab_size);

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
        std::visit(write_param_visitor(&b, &(pair.first)), pair.second);
    }

    out_file << b;
}

} // namespace netlist
} // namespace cbag
