/** \file cellviews.h
 *  \brief This file defines classes representing various cell views.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SCHEMATIC_CELLVIEW_FWD_H
#define CBAG_SCHEMATIC_CELLVIEW_FWD_H

#include <memory>
#include <vector>

#include <cbag/util/sorted_map.h>

#include <cbag/common/box_t.h>
#include <cbag/common/datatypes.h>
#include <cbag/common/typedefs.h>
#include <cbag/schematic/shape_t.h>
#include <cbag/schematic/term_t.h>

namespace cbag {
namespace sch {

struct instance;
struct cellview_info;

using inst_map_t = cbag::util::sorted_map<std::string, std::unique_ptr<instance>>;
using conn_list_t = std::vector<std::pair<std::string, std::string>>;

/** A schematic or symbol cell view
 */
struct cellview {
  public:
    std::string lib_name;
    std::string cell_name;
    std::string view_name;
    box_t bbox;
    term_t terminals;
    std::vector<shape_t> shapes;
    inst_map_t instances;
    param_map props;
    param_map app_defs;
    std::unique_ptr<cellview> sym_ptr;

    cellview();

    explicit cellview(const std::string &fname);

    cellview(std::string lib_name, std::string cell_name, std::string view_name, coord_t xl,
             coord_t yl, coord_t xh, coord_t yh);

    cellview_info get_info(const std::string &cell_name) const;

    void to_file(const std::string &fname) const;

    void clear_params();

    void set_param(std::string name, const std::variant<int32_t, double, bool, std::string> &val);

    void rename_pin(const std::string &old_name, const std::string &new_name);

    void add_pin(const std::string &new_name, uint32_t term_type);

    bool remove_pin(const std::string &name);

    void rename_instance(const std::string &old_name, std::string new_name);

    bool remove_instance(const std::string &name);
};

} // namespace sch
} // namespace cbag

#endif
