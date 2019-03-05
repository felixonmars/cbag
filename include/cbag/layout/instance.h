#ifndef CBAG_LAYOUT_INSTANCE_H
#define CBAG_LAYOUT_INSTANCE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

#include <cbag/common/param_map.h>
#include <cbag/common/transformation.h>
#include <cbag/common/typedefs.h>

namespace cbag {

class box_t;

namespace layout {

// class forwarding
class cellview;

struct cellview_ref {
    std::string lib;
    std::string cell;
    std::string view;
    param_map params;

    cellview_ref(std::string lib, std::string cell, std::string view);

    bool operator==(const cellview_ref &rhs) const noexcept;
};

using str_map_t = std::unordered_map<std::string, std::string>;

class instance {
  private:
    std::variant<std::shared_ptr<const cellview>, cellview_ref> master = nullptr;
    std::string name;

  public:
    cbag::transformation xform;
    cnt_t nx = 1;
    cnt_t ny = 1;
    offset_t spx = 0;
    offset_t spy = 0;

    instance();

    instance(std::string name, std::string lib, std::string cell, std::string view,
             cbag::transformation xform, cnt_t nx = 1, cnt_t ny = 1, offset_t spx = 0,
             offset_t spy = 0);

    instance(std::string name, std::shared_ptr<const cellview> master, cbag::transformation xform,
             cnt_t nx = 1, cnt_t ny = 1, offset_t spx = 0, offset_t spy = 0);

    bool operator==(const instance &rhs) const noexcept;

    bool is_reference() const;

    const cellview *get_cellview() const;

    const std::string &get_inst_name() const;

    const std::string &get_lib_name(const std::string &output_lib) const;

    const std::string &get_cell_name(const str_map_t *rename_map) const;

    const std::string &get_view_name(const std::string &default_view) const;

    const param_map *get_params() const;

    box_t get_bbox(const std::string &layer, const std::string &purpose) const;

    void set_master(const std::shared_ptr<const cellview> &new_master);

    void set_param(const std::string &name, const param_t &val);
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
