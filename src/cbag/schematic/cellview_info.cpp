#include <fmt/core.h>

#include <cbag/logging/logging.h>

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/spirit/ast.h>
#include <cbag/util/name_convert.h>
#include <cbag/util/overload.h>

namespace cbag {
namespace sch {

cellview_info::cellview_info() = default;

cellview_info::cellview_info(std::string lib_name, std::string cell_name, bool is_prim)
    : lib_name(std::move(lib_name)), cell_name(std::move(cell_name)), is_prim(is_prim) {}

const cellview_info &get_cv_info(const netlist_map_t &info_map, const std::string &lib_name,
                                 const std::string &cell_name) {
    auto libmap_iter = info_map.find(lib_name);
    if (libmap_iter == info_map.end()) {
        auto logger = cbag::get_cbag_logger();
        auto msg =
            fmt::format("Cannot find library {} in netlist map for cell {}.", lib_name, cell_name);
        logger->error(msg);
        throw std::invalid_argument(msg);
    }

    auto cellmap_iter = libmap_iter->second.find(cell_name);
    if (cellmap_iter == libmap_iter->second.end()) {
        auto logger = cbag::get_cbag_logger();
        auto msg = fmt::format("Cannot find cell {}__{} in netlist map.", lib_name, cell_name);
        logger->error(msg);
        throw std::invalid_argument(msg);
    }
    return cellmap_iter->second;
}

void record_cv_info(netlist_map_t &info_map, const std::string &lib_name,
                    const std::string &cell_name, cellview_info &&info) {
    auto lib_map_iter = info_map.find(lib_name);
    if (lib_map_iter == info_map.end()) {
        sch::lib_map_t new_lib_map;
        new_lib_map.emplace(cell_name, std::move(info));
        info_map.emplace(lib_name, std::move(new_lib_map));
    } else {
        lib_map_iter->second.emplace(cell_name, std::move(info));
    }
}

using nu_range_map_t = cbag::util::sorted_map<std::string, std::array<cnt_t, 2>>;

const attr_map_t *get_attrs(const util::sorted_map<std::string, attr_map_t> &table,
                            const std::string &base_name) {
    auto iter = table.find(base_name);
    if (iter == table.end())
        return nullptr;
    return &iter->second;
}

void register_unique_name_units(const spirit::ast::name_unit &ast, const attr_map_t *attr_ptr,
                                nu_range_map_t &net_range_map, const nu_range_map_t &term_range_map,
                                util::sorted_map<std::string, attr_map_t> &attr_map) {
    // check that we do not have a conflict in net attribute
    if (attr_ptr) {
        auto attr_ptr_ref = get_attrs(attr_map, ast.base);
        if (attr_ptr_ref) {
            if (*attr_ptr_ref != *attr_ptr) {
                auto msg = fmt::format("Terminal/net {} has conflicting attribute.", ast.base);
                auto logger = cbag::get_cbag_logger();
                logger->error(msg);
                throw std::runtime_error(msg);
            }
        } else {
            // update attribute table
            attr_map.emplace(ast.base, *attr_ptr);
        }
    }

    auto net_bounds = ast.idx_range.bounds();
    auto term_iter = term_range_map.find(ast.base);
    if (term_iter != term_range_map.end()) {
        // this net is connected to a terminal
        // check that net range does not go out of bounds of terminal range
        auto & [ term_start, term_stop ] = term_iter->second;
        auto term_scalar = (term_start == term_stop);
        auto net_scalar = (net_bounds[0] == net_bounds[1]);
        if ((term_scalar != net_scalar) ||
            (!term_scalar && (net_bounds[0] < term_start || net_bounds[1] > term_stop))) {
            auto logger = cbag::get_cbag_logger();
            auto msg =
                fmt::format("Schematic cannot contain net {} because a terminal with the same base "
                            "name and difference bounds exist.",
                            ast.to_string(spirit::namespace_cdba{}));
            logger->error(msg);
            throw std::runtime_error(msg);
        }
    } else {
        // register net
        auto net_iter = net_range_map.find(ast.base);
        if (net_iter == net_range_map.end()) {
            net_range_map.emplace(ast.base, net_bounds);
        } else {
            auto &cur_bnds = net_iter->second;
            cur_bnds[0] = std::min(cur_bnds[0], net_bounds[0]);
            cur_bnds[1] = std::max(cur_bnds[1], net_bounds[1]);
        }
    }
}

void register_unique_name_units(const spirit::ast::name &ast, const attr_map_t *attr_ptr,
                                nu_range_map_t &net_range_map, const nu_range_map_t &term_range_map,
                                util::sorted_map<std::string, attr_map_t> &attr_map);

void register_unique_name_units(const spirit::ast::name_rep &ast, const attr_map_t *attr_ptr,
                                nu_range_map_t &net_range_map, const nu_range_map_t &term_range_map,
                                util::sorted_map<std::string, attr_map_t> &attr_map) {
    std::visit(
        overload{
            [&attr_ptr, &net_range_map, &term_range_map,
             &attr_map](const spirit::ast::name_unit &arg) {
                register_unique_name_units(arg, attr_ptr, net_range_map, term_range_map, attr_map);
            },
            [&attr_ptr, &net_range_map, &term_range_map, &attr_map](const spirit::ast::name &arg) {
                register_unique_name_units(arg, attr_ptr, net_range_map, term_range_map, attr_map);
            },
        },
        ast.data);
}

void register_unique_name_units(const spirit::ast::name &ast, const attr_map_t *attr_ptr,
                                nu_range_map_t &net_range_map, const nu_range_map_t &term_range_map,
                                util::sorted_map<std::string, attr_map_t> &attr_map) {
    for (const auto &nr : ast.rep_list) {
        register_unique_name_units(nr, attr_ptr, net_range_map, term_range_map, attr_map);
    }
}

cellview_info get_cv_netlist_info(const cellview &cv, const std::string &cell_name,
                                  const netlist_map_t &info_map, bool compute_net_attrs) {
    cellview_info ans(cv.lib_name, cell_name, false);

    nu_range_map_t term_range_map, net_range_map;

    // process terminals
    for (auto const & [ term_name, term_fig ] : cv.terminals) {
        switch (term_fig.ttype) {
        case term_type::input:
            ans.in_terms.push_back(term_name);
            break;
        case term_type::output:
            ans.out_terms.push_back(term_name);
            break;
        default:
            // this cellview is validated, term_type guaranteed to be supported
            ans.io_terms.push_back(term_name);
            break;
        }

        // record terminal bus range
        auto ast = util::parse_cdba_name_unit(term_name);
        term_range_map.emplace(ast.base, ast.idx_range.bounds());

        // record terminal attribute
        if (!term_fig.attrs.empty())
            ans.term_net_attrs.emplace(ast.base, term_fig.attrs);
    }

    // get all the nets
    for (auto const & [ inst_name, inst_ptr ] : cv.instances) {
        auto inst_cv_info_ptr =
            (compute_net_attrs) ? &get_cv_info(info_map, inst_ptr->lib_name, inst_ptr->cell_name)
                                : nullptr;
        for (auto const & [ inst_term, net ] : inst_ptr->connections) {
            auto term_ast = util::parse_cdba_name_unit(inst_term);
            auto net_ast = util::parse_cdba_name(net);
            auto attr_ptr = (inst_cv_info_ptr)
                                ? get_attrs(inst_cv_info_ptr->term_net_attrs, term_ast.base)
                                : nullptr;
            register_unique_name_units(net_ast, attr_ptr, net_range_map, term_range_map,
                                       ans.term_net_attrs);
        }
    }

    // save all nets to netlist
    for (auto const & [ net_name, net_bnds ] : net_range_map) {
        ans.nets.push_back(spirit::ast::to_string(net_name, net_bnds, spirit::namespace_cdba{}));
    }

    return ans;
}

} // namespace sch
} // namespace cbag
