#ifndef CBAG_LAYOUT_INSTANCE_H
#define CBAG_LAYOUT_INSTANCE_H

#include <string>
#include <variant>

#include <cbag/common/typedefs.h>
#include <cbag/layout/transformation.h>

namespace cbag {
namespace layout {

// class forwarding
class cellview;

struct cellview_ref {
    std::string lib;
    std::string cell;
    std::string view;

    cellview_ref(const char *lib, const char *cell, const char *view);
};

class instance {
  private:
    transformation xform;
    uint32_t nx = 1;
    uint32_t ny = 1;
    offset_t spx = 0;
    offset_t spy = 0;
    std::variant<const cellview *, cellview_ref> master = nullptr;

  public:
    instance();

    instance(const char *lib, const char *cell, const char *view, transformation xform,
             uint32_t nx = 1, uint32_t ny = 1, offset_t spx = 0, offset_t spy = 0);

    instance(const cellview *master, transformation xform, uint32_t nx = 1, uint32_t ny = 1,
             offset_t spx = 0, offset_t spy = 0);
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
