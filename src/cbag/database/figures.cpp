/** \file figures.cpp
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <cstdlib>

#include <fmt/format.h>

#include <cbag/cbag.h>
#include <cbag/database/figures.h>
#include <cbag/netlist/name_convert.h>

namespace cbag {

void Instance::clear_params() { params.clear(); }

void Instance::set_int_param(const char *name, int value) { params[name] = value; }

void Instance::set_double_param(const char *name, double value) { params[name] = value; }

void Instance::set_bool_param(const char *name, bool value) { params[name] = value; }

void Instance::set_string_param(const char *name, const char *value) {
    params[name] = std::string(value);
}

void Instance::update_connection(const std::string &inst_name, const char *term, const char *net) {
    // check number of bits match
    std::string term_str(term);
    std::string net_str(net);
    spirit::ast::name_unit nu = parse_cdba_name_unit(inst_name);
    spirit::ast::name n_term = parse_cdba_name(term_str);
    spirit::ast::name n_net = parse_cdba_name(net_str);

    uint32_t tot_size = nu.size() * n_term.size();
    uint32_t net_size = n_net.size();
    if (tot_size == net_size) {
        // direct connection
        connections[term_str] = std::move(net_str);
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
        connections[term_str] = to_string_cdba(n_net);
    }
}

void Instance::resize_nets(uint32_t old_size, uint32_t new_size) {
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

} // namespace cbag
