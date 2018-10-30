/** \file cellviews.h
 *  \brief This file defines classes representing various cell views.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SCHEMATIC_CELLVIEW_H
#define CBAG_SCHEMATIC_CELLVIEW_H

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <cbag/common/box_t.h>
#include <cbag/common/datatypes.h>
#include <cbag/common/typedefs.h>
#include <cbag/schematic/shape_t.h>
#include <cbag/schematic/term_t.h>

namespace cbag {
namespace sch {

struct instance;
struct cellview_info;

using conn_list_t = std::vector<std::pair<std::string, std::string>>;
using inst_iter_t = std::map<std::string, instance>::iterator;

/** A schematic or symbol cell view
 */
struct cellview {
  public:
    std::string lib_name;
    std::string cell_name;
    std::string view_name;
    box_t bbox;
    term_t in_terms;
    term_t out_terms;
    term_t io_terms;
    std::vector<shape_t> shapes;
    std::map<std::string, instance> instances;
    param_map props;
    param_map app_defs;
    std::unique_ptr<cellview> sym_ptr;

    cellview();

    cellview(std::string lib_name, std::string cell_name, std::string view_name, box_t bbox);

    cellview_info get_info(const std::string &cell_name) const;

    void clear_params();

    void set_param(std::string name, const std::variant<int32_t, double, bool, std::string> &val);

    void rename_pin(const std::string &old_name, const std::string &new_name);

    void add_pin(const std::string &new_name, uint32_t term_type);

    bool remove_pin(const std::string &name);

    void rename_instance(const std::string &old_name, std::string new_name);

    bool remove_instance(const std::string &name);

    void copy_instance(const instance &inst, uint32_t old_size, const std::string &new_name,
                       coord_t dx, coord_t dy, const conn_list_t &conns);

    void array_instance(const std::string &old_name, const std::vector<std::string> &name_list,
                        coord_t dx, coord_t dy, const std::vector<conn_list_t> &conns_list);
};

} // namespace sch
} // namespace cbag

#endif // CBAG_SCHEMATIC_CELLVIEWS_H
