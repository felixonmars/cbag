#ifndef CBAG_LAYOUT_INSTANCE_H
#define CBAG_LAYOUT_INSTANCE_H

#include <string>
#include <variant>

#include <cbag/common/datatypes.h>
#include <cbag/common/transformation.h>
#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

// class forwarding
class cellview;
class rectangle;

struct cellview_ref {
    std::string lib;
    std::string cell;
    std::string view;
    param_map params;

    cellview_ref(const char *lib, const char *cell, const char *view);
};

using str_map_t = std::unordered_map<std::string, std::string>;

class instance {
  private:
    std::variant<const cellview *, cellview_ref> master = nullptr;

  public:
    const char *name;
    cbag::transformation xform;
    uint32_t nx = 1;
    uint32_t ny = 1;
    offset_t spx = 0;
    offset_t spy = 0;

    instance();

    instance(const char *name, const char *lib, const char *cell, const char *view,
             cbag::transformation xform, uint32_t nx = 1, uint32_t ny = 1, offset_t spx = 0,
             offset_t spy = 0);

    instance(const char *name, const cellview *master, cbag::transformation xform, uint32_t nx = 1,
             uint32_t ny = 1, offset_t spx = 0, offset_t spy = 0);

    bool is_reference() const;

    const cellview *get_cellview() const;

    const char *get_lib_name(const char *output_lib) const;

    const char *get_cell_name(const str_map_t *rename_map) const;

    const char *get_view_name(const char *default_view) const;

    const param_map *get_params() const;

    rectangle get_bbox(const char *layer, const char *purpose) const;

    void set_master(const cellview *new_master);

    void set_int_param(const char *name, int value);

    void set_double_param(const char *name, double value);

    void set_bool_param(const char *name, bool value);

    void set_string_param(const char *name, const char *value);
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
