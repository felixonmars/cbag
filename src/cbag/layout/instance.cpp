#include <cbag/layout/instance.h>

namespace cbag {
namespace layout {

instance::instance(std::string lib, std::string cell, std::string view, transformation xform,
                   uint32_t nx, uint32_t ny, coord_t spx, coord_t spy)
    : lib(std::move(lib)), cell(std::move(cell)), view(std::move(view)), xform(xform), nx(nx),
      ny(ny), spx(spx), spy(spy) {}

} // namespace layout
} // namespace cbag
