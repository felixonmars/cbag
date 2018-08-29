#ifndef CBAG_LAYOUT_INSTANCE_H
#define CBAG_LAYOUT_INSTANCE_H

#include <string>

#include <cbag/common/transform.h>
#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

// class forwarding
class cellview;

class instance {
  private:
    std::string lib;
    std::string cell;
    std::string view;
    transform xform;
    uint32_t nx = 1;
    uint32_t ny = 1;
    offset_t spx = 0;
    offset_t spy = 0;
    cellview *master = nullptr;

  public:
    instance(std::string lib, std::string cell, std::string view, transform xform, uint32_t nx = 1,
             uint32_t ny = 1, coord_t spx = 0, coord_t spy = 0);
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
