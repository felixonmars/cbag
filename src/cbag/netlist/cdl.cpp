/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <boost/range/join.hpp>
#include <boost/variant.hpp>

#include <fmt/format.h>

#include <cbag/database/cellviews.h>
#include <cbag/database/figures.h>
#include <cbag/netlist/cdl.h>

namespace cbag {

void CDLBuilder::init(const std::vector<std::string> &inc_list, bool shell) {
    if (!shell) {
        for (auto const &fname : inc_list) {
            out_file << ".INCLUDE " << fname << std::endl;
        }
        out_file << std::endl;
        out_file << ".PARAM" << std::endl;
        out_file << std::endl;
    }
}

void CDLBuilder::write_end() {}

void CDLBuilder::write_cv_header(const std::string &name,
                                 const term_t &in_terms,
                                 const term_t &out_terms,
                                 const term_t &io_terms) {
    LineBuilder b(ncol, cnt_char, break_before, tab_size);
    b << ".SUBCKT";
    b << name;
    auto tmp_range = boost::join(in_terms, out_terms);
    for (auto const &pair : boost::join(tmp_range, io_terms)) {
        spirit::ast::name ast = parse_cdba_name(pair.first);
        for (auto const &bit : ast) {
            b << to_string_cdba(bit);
        }
    }

    out_file << b;
}

void CDLBuilder::write_cv_end(const std::string &name) {
    out_file << ".ENDS" << std::endl;
}

void CDLBuilder::write_instance_helper(const std::string &name,
                                       const Instance &inst,
                                       const SchCellViewInfo &info) {
    LineBuilder b(ncol, cnt_char, break_before, tab_size);

    // <name> <net1> <net2> ... <cell name> <par1>=<val1> ...
    // write instance name
    b << name;

    // write instance connections
    auto tmp_range = boost::join(info.in_terms, info.out_terms);
    for (auto const &term : boost::join(tmp_range, info.io_terms)) {
        auto term_iter = inst.connections.find(term);
        if (term_iter == inst.connections.end()) {
            throw std::invalid_argument(
                fmt::format("Cannot find net {} in cellview {}__{}", term,
                            inst.lib_name, inst.cell_name));
        }
        b << term_iter->second;
    }

    // write instance cell name
    if (!info.is_prim) {
        b << "/";
    }
    b << inst.cell_name;

    // get default parameter values
    ParamMap par_map(info.props);
    // update with instance parameters
    for (auto const &pair : inst.params) {
        par_map[pair.first] = pair.second;
    }
    // write instance parameters
    for (auto const &pair : par_map) {
        boost::apply_visitor(write_param_visitor(&b, &(pair.first)),
                             pair.second);
    }

    out_file << b;
}

} // namespace cbag
