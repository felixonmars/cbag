/** \file figures.h
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#ifndef CBAG_SCHEMATIC_INSTANCE_H
#define CBAG_SCHEMATIC_INSTANCE_H

#include <cbag/common/box_t.h>
#include <cbag/common/datatypes.h>
#include <cbag/common/transform.h>

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
    transform xform;
    box_t bbox;
    bool is_primitive = false;
    cbag::util::sorted_map<std::string, std::string> connections;
    param_map params;

    instance();

    /** Create an instance with empty parameter and terminal mappings.
     *
     * @param lib the library name.
     * @param cell the cell name.
     * @param view the view name.
     * @param xform the instance location.
     */
    instance(std::string lib, std::string cell, std::string view, transform xform, box_t bbox);

    uint32_t width() const;

    uint32_t height() const;

    void clear_params();

    void set_param(std::string &&name, const std::variant<int32_t, double, bool, std::string> &val);

    void update_connection(const std::string &inst_name, uint32_t inst_size, std::string term_str,
                           std::string net_str);

    void update_connection(const std::string &inst_name, std::string &&term, std::string &&net);

    void resize_nets(uint32_t old_size, uint32_t new_size);
};

} // namespace sch
} // namespace cbag

#endif
