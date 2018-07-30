/** \file cellviews.cpp
 *  \brief This file defines classes representing various cellviews
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#include <fmt/format.h>

#include <cbag/database/yaml_cellviews.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/name_unit.h>
#include <cbag/spirit/parsers.h>

namespace cbag {
SchCellView::SchCellView(const std::string &yaml_fname) {
    YAML::Node n = YAML::LoadFile(yaml_fname);
    (*this) = n.as<SchCellView>();
}

void SchCellView::clear_params() { props.clear(); }

void SchCellView::set_int_param(const char *name, int value) {
    props.insert_or_assign(name, value);
}

void SchCellView::set_double_param(const char *name, double value) {
    props.insert_or_assign(name, value);
}

void SchCellView::set_bool_param(const char *name, bool value) {
    props.insert_or_assign(name, value);
}

void SchCellView::set_string_param(const char *name, const char *value) {
    props.insert_or_assign(name, value);
}

void SchCellView::rename_pin(const char *old_name, const char *new_name) {
    // check the new pin name does not exist already
    std::string nkey(new_name);
    if (in_terms.find(nkey) != in_terms.end() ||
        out_terms.find(nkey) != out_terms.end() ||
        io_terms.find(nkey) != io_terms.end()) {
        throw std::invalid_argument(
            fmt::format("Terminal {} already exists.", nkey));
    }
    // check the new name is legal.  Parse will throw exception if not passed
    spirit::ast::name ast;
    parse(new_name, nkey.size(), spirit::name(), ast);

    std::string key(old_name);
    auto nh = in_terms.extract(key);
    if (nh.empty()) {
        nh = out_terms.extract(key);
        if (nh.empty()) {
            nh = io_terms.extract(key);
            if (nh.empty()) {
                throw std::invalid_argument("Cannot find terminal: " + key);
            }
            nh.key() = std::string(new_name);
            io_terms.insert(std::move(nh));
        } else {
            nh.key() = std::string(new_name);
            out_terms.insert(std::move(nh));
        }
    } else {
        nh.key() = std::string(new_name);
        in_terms.insert(std::move(nh));
    }
}

void SchCellView::add_pin(const char *new_name, uint32_t term_type) {
    std::string key(new_name);
    // check the pin name is legal.  Parse will throw exception if not passed
    spirit::ast::name ast;
    parse(new_name, key.size(), spirit::name(), ast);

    // check the pin name does not exist already
    if (in_terms.find(key) != in_terms.end() ||
        out_terms.find(key) != out_terms.end() ||
        io_terms.find(key) != io_terms.end()) {
        throw std::invalid_argument(
            fmt::format("Terminal {} already exists.", key));
    }

    // get the map to insert
    std::map<std::string, PinFigure> *ptr = nullptr;
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
        throw std::invalid_argument(
            fmt::format("Bad terminal type: {}", term_type));
    }

    // insert into map
    // TODO: calculate new pin figure correctly
    ptr->emplace(std::move(key),
                 PinFigure(Rect(0, 0, "", 0, 0, 10, 10), stSignal));
}

bool SchCellView::remove_pin(const char *name) {
    std::string key(name);
    return in_terms.erase(key) > 0 || out_terms.erase(key) > 0 ||
           io_terms.erase(key) > 0;
}

void SchCellView::rename_instance(const char *old_name, const char *new_name) {
    // check the new name does not exist
    std::string nkey(new_name);
    if (instances.find(nkey) != instances.end()) {
        throw std::invalid_argument(
            fmt::format("Instance {} already exists.", nkey));
    }
    // check the new name is legal.  Parse will throw exception if not passed
    spirit::ast::name_unit ast;
    parse(new_name, nkey.size(), spirit::name_unit(), ast);

    // do the swap
    std::string key(old_name);
    auto nh = instances.extract(key);
    if (nh.empty()) {
        throw std::invalid_argument("Cannot find instance: " + key);
    } else {
        nh.key() = std::string(new_name);
        instances.insert(std::move(nh));
    }
}

bool SchCellView::remove_instance(const char *name) {
    std::string key(name);
    return instances.erase(key) > 0;
}

inst_iter_t SchCellView::copy_instance(const char *old_name,
                                       const std::string &new_name, coord_t dx,
                                       coord_t dy, const conn_list_t &conns) {
    // find the instance to copy
    std::string key(old_name);
    std::map<std::string, Instance>::const_iterator iter = instances.find(key);
    if (iter == instances.end()) {
        throw std::invalid_argument("Cannot find instance: " + key);
    }

    // check the new name is legal.  Parse will throw exception if not passed
    spirit::ast::name_unit new_nu;
    parse(new_name.c_str(), new_name.size(), spirit::name_unit(), new_nu);

    // create new copy
    auto emp_iter = instances.emplace(new_name, iter->second);
    if (!emp_iter.second) {
        throw std::invalid_argument(
            fmt::format("Instance {} already exists.", emp_iter.first->first));
    }

    // shift and update connections
    emp_iter.first->second.xform.xOffset() += dx;
    emp_iter.first->second.xform.xOffset() += dy;
    for (auto const &p : conns) {
        emp_iter.first->second.connections.insert_or_assign(p.first, p.second);
    }
    return emp_iter.first;
}

std::vector<inst_iter_t> SchCellView::array_instance(
    const char *old_name, const std::vector<std::string> &name_list, coord_t dx,
    coord_t dy, const std::vector<conn_list_t> &conns_list) {
    size_t num = name_list.size();
    std::vector<inst_iter_t> ans(num);
    auto num_inst = static_cast<coord_t>(num);
    for (coord_t idx = 0; idx < num_inst; ++idx) {
        ans[idx] = copy_instance(old_name, name_list[idx], dx * idx, dy * idx,
                                 conns_list[idx]);
    }
    return ans;
}

netlist_map_t load_netlist_map(const char *fname) {
    YAML::Node n = YAML::LoadFile(fname);
    return n.as<netlist_map_t>();
}

} // namespace cbag
