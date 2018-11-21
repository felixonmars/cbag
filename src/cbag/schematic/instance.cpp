/** \file figures.cpp
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <cstdlib>

#include <fmt/core.h>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/namespace_info.h>

#include <cbag/util/name_convert.h>

#include <cbag/schematic/instance.h>

namespace cbag {
namespace sch {

instance::instance() = default;

instance::instance(std::string lib, std::string cell, std::string view, transformation xform,
                   coord_t xl, coord_t yl, coord_t xh, coord_t yh)
    : lib_name(std::move(lib)), cell_name(std::move(cell)), view_name(std::move(view)),
      xform(std::move(xform)), bbox(xl, yl, xh, yh), connections(), params() {}

uint32_t instance::width() const { return bbox.w(); }

uint32_t instance::height() const { return bbox.h(); }

void instance::clear_params() { params.clear(); }

void instance::set_param(const std::string &name,
                         const std::variant<int32_t, double, bool, std::string> &val) {
    cbag::set_param(params, name, val);
}

void instance::update_connection(const std::string &inst_name, uint32_t inst_size,
                                 std::string term_str, std::string net_str) {
    // check number of bits match
    spirit::ast::name_unit n_term = cbag::util::parse_cdba_name_unit(term_str);
    spirit::ast::name n_net = cbag::util::parse_cdba_name(net_str);

    uint32_t tot_size = inst_size * n_term.size();
    uint32_t net_size = n_net.size();
    if (tot_size == net_size) {
        // direct connection
        connections.insert_or_assign(std::move(term_str), std::move(net_str));
    } else {
        std::ldiv_t result = std::div(static_cast<long>(tot_size), static_cast<long>(net_size));

        if (result.rem != 0) {
            // cannot broadcast net
            throw std::invalid_argument(fmt::format(
                "Cannot connect instance {} terminal {} to net {}", inst_name, term_str, net_str));
        }
        // broadcast net
        n_net.repeat(result.quot);
        connections.insert_or_assign(std::move(term_str),
                                     n_net.to_string(spirit::namespace_cdba{}));
    }
}

void instance::update_connection(const std::string &inst_name, std::string term, std::string net) {
    // check number of bits match
    spirit::ast::name_unit nu = cbag::util::parse_cdba_name_unit(inst_name);
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
    std::ldiv_t result = std::div(static_cast<long>(new_size), static_cast<long>(old_size));
    if (result.rem != 0) {
        // new size not multiple of old size, just clear connections
        connections.clear();
    } else {
        // repeat all nets
        for (auto &pair : connections) {
            spirit::ast::name net = cbag::util::parse_cdba_name(pair.second);
            pair.second =
                net.repeat(static_cast<uint32_t>(result.quot)).to_string(spirit::namespace_cdba{});
        }
    }
}

} // namespace sch
} // namespace cbag
