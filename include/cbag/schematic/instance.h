/** \file figures.h
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#ifndef CBAG_SCHEMATIC_INSTANCE_H
#define CBAG_SCHEMATIC_INSTANCE_H

#include <memory>

#include <cbag/common/box_t.h>
#include <cbag/common/param_map.h>
#include <cbag/common/transformation.h>

#include <cbag/util/sorted_map.h>

namespace cbag {
namespace sch {

/** An instance object
 */
struct instance {
  public:
    std::string lib_name;
    std::string cell_name;
    std::string view_name;
    transformation xform;
    box_t bbox;
    bool is_primitive = false;
    cbag::util::sorted_map<std::string, std::string> connections;
    param_map params;

    instance();

    instance(std::string lib, std::string cell, std::string view, transformation xform, coord_t xl,
             coord_t yl, coord_t xh, coord_t yh);

    uint32_t width() const;

    uint32_t height() const;

    std::unique_ptr<instance> get_copy() const;

    void clear_params();

    void set_param(const std::string &name, const param_t &val);

    void update_connection(const std::string &inst_name, uint32_t inst_size, std::string term_str,
                           std::string net_str);

    void update_connection(const std::string &inst_name, std::string term, std::string net);

    void update_master(std::string lib, std::string cell, bool prim = false);

    void resize_nets(uint32_t old_size, uint32_t new_size);
};

} // namespace sch
} // namespace cbag

#endif
