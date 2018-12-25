#ifndef CBAG_LAYOUT_INSTANCE_H
#define CBAG_LAYOUT_INSTANCE_H

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
};

using str_map_t = std::unordered_map<std::string, std::string>;

class instance {
  private:
    std::variant<const cellview *, cellview_ref> master = nullptr;

  public:
    const std::string name;
    cbag::transformation xform;
    uint32_t nx = 1;
    uint32_t ny = 1;
    offset_t spx = 0;
    offset_t spy = 0;

    instance();

    instance(std::string name, std::string lib, std::string cell, std::string view,
             cbag::transformation xform, uint32_t nx = 1, uint32_t ny = 1, offset_t spx = 0,
             offset_t spy = 0);

    instance(std::string name, const cellview *master, cbag::transformation xform, uint32_t nx = 1,
             uint32_t ny = 1, offset_t spx = 0, offset_t spy = 0);

    bool is_reference() const;

    const cellview *get_cellview() const;

    const std::string &get_lib_name(const std::string &output_lib) const;

    const std::string &get_cell_name(const str_map_t *rename_map) const;

    const std::string &get_view_name(const std::string &default_view) const;

    const param_map *get_params() const;

    box_t get_bbox(const std::string &layer, const std::string &purpose) const;

    void set_master(const cellview *new_master);

    void set_param(const std::string &name, const param_t &val);
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
