/** \file cellviews.h
 *  \brief This file defines classes representing various cell views.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SCHEMATIC_CELLVIEW_INST_MOD_H
#define CBAG_SCHEMATIC_CELLVIEW_INST_MOD_H

#include <fmt/format.h>

#include <cbag/spirit/name.h>
#include <cbag/spirit/name_unit.h>
#include <cbag/spirit/parsers.h>

#include <cbag/schematic/cellview.h>
#include <cbag/schematic/instance.h>

namespace cbag {
namespace sch {

template <class ConnRange>
void copy_instance(inst_map_t &instances, const instance &inst, uint32_t old_size,
                   const std::string &new_name, coord_t dx, coord_t dy, const ConnRange &conns) {
    // check the new name is legal.  Parse will throw exception if not passed
    spirit::ast::name_unit new_ast;
    parse(new_name, spirit::name_unit(), new_ast);

    // insert with empty pointer
    auto emp_iter = instances.emplace(new_name, std::unique_ptr<instance>());
    if (!emp_iter.second) {
        throw std::invalid_argument(
            fmt::format("instance {} already exists.", emp_iter.first->first));
    }
    // insert successful, make pointer point at something
    emp_iter.first->second = std::make_unique<instance>(inst);
    instance *cur_ptr = emp_iter.first->second.get();

    // resize nets
    uint32_t new_size = new_ast.size();
    if (old_size != new_size) {
        cur_ptr->resize_nets(old_size, new_size);
    }

    // shift and update connections
    cur_ptr->xform.xOffset() += dx;
    cur_ptr->xform.yOffset() += dy;
    for (auto const &p : conns) {
        cur_ptr->update_connection(new_name, new_ast.size(), p.first, p.second);
    }
}

template <class NameConnRange>
void array_instance(inst_map_t &instances, const std::string &old_name, coord_t dx, coord_t dy,
                    const NameConnRange &name_conn_range) {
    // find the instance to copy
    auto iter = instances.find(old_name);
    if (iter == instances.end()) {
        throw std::invalid_argument("Cannot find instance: " + old_name);
    }
    // get old instance name and size
    spirit::ast::name_unit old_ast;
    parse(old_name, spirit::name_unit(), old_ast);
    uint32_t old_size = old_ast.size();

    if (dx == 0 && dy == 0) {
        // figure out default shift
        dx = iter->second->bbox.getWidth() + 10;
    }

    coord_t x = 0;
    coord_t y = 0;
    for (const auto &p : name_conn_range) {
        copy_instance(instances, *(iter->second), old_size, p.first, x, y, p.second);
        x += dx;
        y += dy;
    }

    // remove original instance
    instances.erase(old_name);
}

} // namespace sch
} // namespace cbag

#endif // CBAG_SCHEMATIC_CELLVIEWS_H
