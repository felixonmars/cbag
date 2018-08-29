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

namespace cbag {
namespace sch {

/** An instance object
 */
struct instance {
  public:
    std::string lib_name{};
    std::string cell_name{};
    std::string view_name{};
    transform xform{};
    box_t bbox{};
    bool is_primitive = false;
    std::map<std::string, std::string> connections{};
    param_map params{};

    instance();

    /** Create an instance with empty parameter and terminal mappings.
     *
     * @param lib the library name.
     * @param cell the cell name.
     * @param view the view name.
     * @param xform the instance location.
     */
    instance(std::string lib, std::string cell, std::string view, transform xform, box_t bbox);

    // methods to manipulate parameters, so Cython doesn't have to worry about
    // variants

    void clear_params();

    void set_int_param(const char *name, int value);

    void set_double_param(const char *name, double value);

    void set_bool_param(const char *name, bool value);

    void set_string_param(const char *name, const char *value);

    void update_connection(const std::string &inst_name, uint32_t inst_size, const char *term,
                           const char *net);

    void update_connection(const std::string &inst_name, const char *term, const char *net);

    void resize_nets(uint32_t old_size, uint32_t new_size);

    uint32_t width() const { return bbox.getWidth(); }

    uint32_t height() const { return bbox.getHeight(); }
};

} // namespace sch
} // namespace cbag

#endif
