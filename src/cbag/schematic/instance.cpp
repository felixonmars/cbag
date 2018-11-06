/** \file figures.cpp
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <cstdlib>

#include <fmt/format.h>

#include <cbag/spirit/ast.h>

#include <cbag/netlist/name_convert.h>

#include <cbag/schematic/instance.h>

namespace cbag {
namespace sch {

instance::instance() = default;

instance::instance(std::string lib, std::string cell, std::string view, transformation xform,
                   box_t bbox)
    : lib_name(std::move(lib)), cell_name(std::move(cell)), view_name(std::move(view)),
      xform(std::move(xform)), bbox(std::move(bbox)), connections(), params() {}

uint32_t instance::width() const { return bbox.width(); }

uint32_t instance::height() const { return bbox.height(); }

void instance::clear_params() { params.clear(); }

void instance::set_param(const std::string &name,
                         const std::variant<int32_t, double, bool, std::string> &val) {
    cbag::set_param(params, name, val);
}

void instance::update_connection(const std::string &inst_name, uint32_t inst_size,
                                 std::string term_str, std::string net_str) {
    // check number of bits match
    spirit::ast::name n_term = parse_cdba_name(term_str);
    spirit::ast::name n_net = parse_cdba_name(net_str);

    uint32_t tot_size = inst_size * n_term.size();
    uint32_t net_size = n_net.size();
    if (tot_size == net_size) {
        // direct connection
        connections.insert_or_assign(std::move(term_str), std::move(net_str));
    } else if (tot_size % net_size != 0) {
        // cannot broadcast net
        throw std::invalid_argument(fmt::format("Cannot connect instance {} terminal {} to net {}",
                                                inst_name, term_str, net_str));
    } else {
        // broadcast net
        std::size_t old_cnt = n_net.unit_list.size();
        uint32_t mult = tot_size / net_size;
        n_net.unit_list.reserve(mult * old_cnt);
        for (uint32_t c = 0; c < mult - 1; ++c) {
            std::copy_n(n_net.unit_list.begin(), old_cnt, std::back_inserter(n_net.unit_list));
        }
        connections.insert_or_assign(std::move(term_str), to_string_cdba(n_net));
    }
}

void instance::update_connection(const std::string &inst_name, std::string term, std::string net) {
    // check number of bits match
    spirit::ast::name_unit nu = parse_cdba_name_unit(inst_name);
    update_connection(inst_name, nu.size(), std::move(term), std::move(net));
}

void instance::update_master(std::string lib, std::string cell, bool prim) {
    lib_name = std::move(lib);
    cell_name = std::move(cell);
    is_primitive = prim;

    clear_params();
    connections.clear();
}

void instance::resize_nets(uint32_t old_size, uint32_t new_size) {
    std::div_t result = std::div(static_cast<int>(new_size), static_cast<int>(old_size));
    if (result.rem != 0) {
        // new size not multiple of old size, just clear connections
        connections.clear();
    } else {
        // repeat all nets
        for (auto &pair : connections) {
            spirit::ast::name net = parse_cdba_name(pair.second);
            std::size_t old_cnt = net.unit_list.size();
            net.unit_list.reserve(result.quot * old_cnt);
            for (int c = 0; c < result.quot - 1; ++c) {
                std::copy_n(net.unit_list.begin(), old_cnt, std::back_inserter(net.unit_list));
            }
            pair.second = to_string_cdba(net);
        }
    }
}

} // namespace sch
} // namespace cbag
