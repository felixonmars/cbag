/** \file figures.cpp
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <fmt/format.h>

#include <cbag/cbag.h>
#include <cbag/database/figures.h>
#include <cbag/netlist/name_convert.h>

namespace cbag {

void Instance::clear_params() { params.clear(); }

void Instance::set_int_param(const char *name, int value) {
    params[name] = value;
}

void Instance::set_double_param(const char *name, double value) {
    params[name] = value;
}

void Instance::set_bool_param(const char *name, bool value) {
    params[name] = value;
}

void Instance::set_string_param(const char *name, const char *value) {
    params[name] = std::string(value);
}

void Instance::update_connection(const std::string &inst_name, const char *term,
                                 const char *net) {
    // check number of bits match
    std::string term_str(term);
    std::string net_str(net);
    spirit::ast::name_unit nu = parse_cdba_name_unit(inst_name);
    spirit::ast::name n_term = parse_cdba_name(term_str);
    spirit::ast::name n_net = parse_cdba_name(net_str);
    if (nu.size() * n_term.size() != n_net.size()) {
        throw std::invalid_argument(
            fmt::format("Cannot connect instance {} terminal {} to net {}",
                        inst_name, term_str, net_str));
    }

    // overwrite
    connections[term_str] = std::move(net_str);
}
} // namespace cbag
