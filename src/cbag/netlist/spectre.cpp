/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Ayan Biswas
 *  \date   2019/02/04
 */

#include <map>
#include <utility>
#include <variant>

#include <fmt/core.h>

#include <cbag/spirit/ast.h>

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/schematic/instance.h>

#include <cbag/netlist/lstream.h>
#include <cbag/netlist/spectre.h>
#include <cbag/spirit/util.h>
#include <cbag/util/io.h>
#include <cbag/util/name_convert.h>

namespace cbag {
namespace netlist {

spectre_stream::spectre_stream(const std::string &fname, cnt_t rmin)
    : nstream_file(fname), rmin(rmin) {
    // map CDF cell names to Spectre cell names
    cdf2spectre_names["cap"] = "capacitor";
    cdf2spectre_names["idc"] = "isource";
    cdf2spectre_names["ipulse"] = "isource";
    cdf2spectre_names["isin"] = "isource";
    cdf2spectre_names["res"] = "resistor";
    cdf2spectre_names["vdc"] = "vsource";
    cdf2spectre_names["vpulse"] = "vsource";
    cdf2spectre_names["vsin"] = "vsource";

    // map CDF properties to Spectre properties
    cdf2spectre_props["acm"] = "mag";
    cdf2spectre_props["acp"] = "phase";
    cdf2spectre_props["i1"] = "val0";
    cdf2spectre_props["i2"] = "val1";
    cdf2spectre_props["ia"] = "ampl";
    cdf2spectre_props["idc"] = "dc";
    cdf2spectre_props["per"] = "period";
    cdf2spectre_props["pw"] = "width";
    cdf2spectre_props["srcType"] = "type";
    cdf2spectre_props["td"] = "delay";
    cdf2spectre_props["v1"] = "val0";
    cdf2spectre_props["v2"] = "val1";
    cdf2spectre_props["va"] = "ampl";
    cdf2spectre_props["vdc"] = "dc";
}

void traits::nstream<spectre_stream>::close(type &stream) { stream.close(); }

void traits::nstream<spectre_stream>::write_header(type &stream,
                                                   const std::vector<std::string> &inc_list,
                                                   bool shell) {
    if (!shell) {
        if (!inc_list.empty()) {
            for (auto const &fname : inc_list) {
                stream.out_file << "include \"" << util::get_canonical_path(fname).c_str() << '"'
                                << std::endl;
            }
            stream.out_file << std::endl;
        }
    }
    // set language
    stream.out_file << "simulator lang=spectre" << std::endl;
}

void traits::nstream<spectre_stream>::write_end(type &stream) {}

void get_cv_pins(lstream &b, const std::vector<std::string> &names) {
    for (auto const &name : names) {
        spirit::ast::name_unit ast = cbag::util::parse_cdba_name_unit(name);
        auto n = ast.size();
        for (decltype(n) idx = 0; idx < n; ++idx) {
            std::string tmp = ast.get_name_bit(idx, false, spirit::namespace_cdba{});
            b << tmp;
        }
    }
}
void traits::nstream<spectre_stream>::write_cv_header(type &stream, const std::string &name,
                                                      const sch::cellview_info &info, bool shell,
                                                      bool write_subckt) {
    stream.out_file << std::endl << std::endl;
    if (write_subckt) {
        lstream b;
        b << "subckt";
        b << name;
        get_cv_pins(b, info.in_terms);
        get_cv_pins(b, info.out_terms);
        get_cv_pins(b, info.io_terms);

        // write definition line
        b.to_file(stream.out_file, spirit::namespace_cdba{});
    }
}

void traits::nstream<spectre_stream>::write_cv_end(type &stream, const std::string &name,
                                                   bool write_subckt) {
    if (write_subckt) {
        stream.out_file << "ends " << name << std::endl;
    } else {
        stream.out_file << std::endl;
    }
}

void append_nets1(lstream &b, const std::string &inst_name, const sch::instance &inst,
                  const std::vector<std::string> &terms) {
    for (auto const &term : terms) {
        auto term_iter = inst.connections.find(term);
        if (term_iter == inst.connections.end()) {
            throw std::invalid_argument(fmt::format(
                "Cannot find net connected to instance {} terminal {}", inst_name, term));
        }
        spirit::ast::name ast = cbag::util::parse_cdba_name(term_iter->second);
        spirit::util::get_name_bits(ast, b.get_back_inserter(), spirit::namespace_cdba{});
    }
}

template <class OutIter>
void write_instance_cell_name1(OutIter &&iter, const sch::instance &inst,
                               const sch::cellview_info &info, spectre_stream &stream) {
    if (inst.lib_name == "analogLib") {
        // change CDF names to Spectre names for analogLib cells
        auto itr = stream.cdf2spectre_names.find(inst.cell_name);
        if (itr == stream.cdf2spectre_names.end()) {
            *iter = inst.cell_name;
        } else {
            *iter = itr->second;
        }
    } else {
        *iter = inst.cell_name;
    }

    // get default parameter values
    param_map par_map(info.props);
    // update with instance parameters
    for (auto const &pair : inst.params) {
        par_map.insert_or_assign(pair.first, pair.second);
    }

    // write instance parameters
    if (inst.lib_name == "analogLib") {
        for (auto const &pair : par_map) {
            // map CDF parameters to Spectre parameters
            auto itr2 = stream.cdf2spectre_props.find(pair.first);
            if (itr2 == stream.cdf2spectre_props.end()) {
                std::visit(write_param_visitor(iter, pair.first), pair.second);
            } else {
                std::visit(write_param_visitor(iter, itr2->second), pair.second);
            }
        }
    } else {
        for (auto const &pair : par_map) {
            std::visit(write_param_visitor(iter, pair.first), pair.second);
        }
    }
}

using term_net_vec_t = std::vector<std::pair<cnt_t, std::vector<std::string>>>;

void get_term_net_pairs1(term_net_vec_t &term_net_vec, const std::string &inst_name,
                         const sch::instance &inst, const std::vector<std::string> &terms) {
    for (const auto &term : terms) {
        auto term_iter = inst.connections.find(term);
        if (term_iter == inst.connections.end()) {
            throw std::invalid_argument(fmt::format(
                "Cannot find net connected to instance {} terminal {}", inst_name, term));
        }
        spirit::ast::name_unit ast_term = cbag::util::parse_cdba_name_unit(term);
        spirit::ast::name ast = cbag::util::parse_cdba_name(term_iter->second);
        std::vector<std::string> net_vec;
        net_vec.reserve(ast.size());
        spirit::util::get_name_bits(ast, std::back_inserter(net_vec), spirit::namespace_cdba{});
        term_net_vec.emplace_back(ast_term.size(), std::move(net_vec));
    }
}

void traits::nstream<spectre_stream>::write_instance(type &stream, const std::string &name,
                                                     const sch::instance &inst,
                                                     const sch::cellview_info &info) {
    spirit::ast::name_unit inst_ast = cbag::util::parse_cdba_name_unit(name);
    auto n = inst_ast.size();

    if (n == 1) {
        // normal instance, just write normally
        lstream b;
        b << name;
        append_nets1(b, name, inst, info.in_terms);
        append_nets1(b, name, inst, info.out_terms);
        append_nets1(b, name, inst, info.io_terms);
        cbag::netlist::write_instance_cell_name1(b.get_back_inserter(), inst, info, stream);
        b.to_file(stream.out_file, spirit::namespace_cdba{});
    } else {
        // arrayed instance, need to split up
        // get name bits of terminals/nets
        term_net_vec_t term_net_vec;
        get_term_net_pairs1(term_net_vec, name, inst, info.in_terms);
        get_term_net_pairs1(term_net_vec, name, inst, info.out_terms);
        get_term_net_pairs1(term_net_vec, name, inst, info.io_terms);
        // get cell name tokens
        std::vector<std::string> tokens;
        tokens.reserve(2);
        cbag::netlist::write_instance_cell_name1(std::back_inserter(tokens), inst, info, stream);
        // array instance
        for (decltype(n) inst_idx = 0; inst_idx < n; ++inst_idx) {
            lstream b;
            // write instance name
            b << inst_ast.get_name_bit(inst_idx, true, spirit::namespace_cdba{});
            // write instance nets
            for (const auto &pair : term_net_vec) {
                auto term_n = pair.first;
                auto net_idx = inst_idx * term_n;
                auto stop_idx = net_idx + term_n;
                for (; net_idx < stop_idx; ++net_idx) {
                    b << pair.second[net_idx];
                }
            }
            // write instance cell name
            b << tokens;
            b.to_file(stream.out_file, spirit::namespace_cdba{});
        }
    }
}

void traits::nstream<spectre_stream>::append_netlist(type &stream, const std::string &netlist) {
    stream.out_file << std::endl << std::endl;
    stream.out_file << netlist;
}

} // namespace netlist
} // namespace cbag
