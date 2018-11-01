/** \file cellviews.cpp
 *  \brief This file defines classes representing various cellviews
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#include <fmt/format.h>

#include <cbag/spirit/name.h>
#include <cbag/spirit/name_unit.h>
#include <cbag/spirit/parsers.h>

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/schematic/instance.h>
#include <cbag/schematic/pin_figure.h>
#include <cbag/schematic/shape_t_def.h>

namespace cbag {
namespace sch {

cellview::cellview() = default;

cellview::cellview(std::string lib_name, std::string cell_name, std::string view_name, box_t bbox)
    : lib_name(std::move(lib_name)), cell_name(std::move(cell_name)),
      view_name(std::move(view_name)), bbox(std::move(bbox)) {}

cellview_info cellview::get_info(const std::string &name) const {
    cellview_info ans(name, in_terms.size(), out_terms.size(), io_terms.size(), false);

    for (auto const &pair : in_terms) {
        ans.in_terms.push_back(pair.first);
    }

    for (auto const &pair : out_terms) {
        ans.out_terms.push_back(pair.first);
    }

    for (auto const &pair : io_terms) {
        ans.io_terms.push_back(pair.first);
    }

    return ans;
}

void cellview::clear_params() { props.clear(); }

void cellview::set_param(std::string name,
                         const std::variant<int32_t, double, bool, std::string> &val) {
    cbag::set_param(props, std::move(name), val);
}

void cellview::rename_pin(const std::string &old_name, const std::string &new_name) {
    // check the new pin name does not exist already
    if (in_terms.find(new_name) != in_terms.end() || out_terms.find(new_name) != out_terms.end() ||
        io_terms.find(new_name) != io_terms.end()) {
        throw std::invalid_argument(fmt::format("Terminal {} already exists.", new_name));
    }
    // check the new name is legal.  Parse will throw exception if not passed
    spirit::ast::name ast;
    parse(new_name, spirit::name(), ast);

    if (in_terms.replace_key(old_name, new_name) == in_terms.end() &&
        out_terms.replace_key(old_name, new_name) == out_terms.end() &&
        io_terms.replace_key(old_name, new_name) == io_terms.end()) {
        throw std::invalid_argument("Cannot find terminal: " + old_name);
    }

    // rename the corresponding symbol pin
    if (sym_ptr != nullptr) {
        sym_ptr->rename_pin(old_name, new_name);
    }
}

void cellview::add_pin(const std::string &new_name, uint32_t term_type) {
    // check the pin name is legal.  Parse will throw exception if not passed
    spirit::ast::name ast;
    parse(new_name, spirit::name(), ast);

    // check the pin name does not exist already
    if (in_terms.find(new_name) != in_terms.end() || out_terms.find(new_name) != out_terms.end() ||
        io_terms.find(new_name) != io_terms.end()) {
        throw std::invalid_argument(fmt::format("Terminal {} already exists.", new_name));
    }
    /*
    // get the map to insert
    cbag::util::sorted_map<std::string, PinFigure> *ptr = nullptr;
    switch (term_type) {
    case trmInput:
        ptr = &in_terms;
        break;
    case trmOutput:
        ptr = &out_terms;
        break;
    case trmInout:
        ptr = &io_terms;
        break;
    default:
        throw std::invalid_argument(fmt::format("Bad terminal type: {}", term_type));
    }

    // insert into map
    ptr->emplace(std::move(key), PinFigure(Rect(0, 0, "", 0, 0, 10, 10), stSignal));
    */

    // TODO: add implementation
    throw std::runtime_error("add_pin functionality not implemented yet.  See developer.");
}

bool cellview::remove_pin(const std::string &name) {
    bool success =
        in_terms.erase(name) > 0 || out_terms.erase(name) > 0 || io_terms.erase(name) > 0;
    // remove symbol pin
    if (success && sym_ptr != nullptr) {
        sym_ptr->remove_pin(name);
    }
    return success;
}

void cellview::rename_instance(const std::string &old_name, std::string new_name) {
    // check the new name does not exist
    if (instances.find(new_name) != instances.end()) {
        throw std::invalid_argument(fmt::format("instance {} already exists.", new_name));
    }
    // check the new name is legal.  Parse will throw exception if not passed
    spirit::ast::name_unit new_ast;
    parse(new_name, spirit::name_unit(), new_ast);

    auto iter = instances.replace_key(old_name, new_name);
    if (iter == instances.end())
        throw std::invalid_argument("Cannot find instance: " + old_name);

    // resize nets if necessary
    spirit::ast::name_unit old_ast;
    parse(old_name, spirit::name_unit(), old_ast);
    uint32_t old_size = old_ast.size();
    uint32_t new_size = new_ast.size();
    if (old_size != new_size) {
        iter->second.resize_nets(old_size, new_size);
    }
}

bool cellview::remove_instance(const std::string &name) { return instances.erase(name) > 0; }

} // namespace sch
} // namespace cbag
