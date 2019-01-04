#ifndef CBAG_LAYOUT_ROUTING_GRID_H
#define CBAG_LAYOUT_ROUTING_GRID_H

#include <cbag/common/typedefs.h>
#include <cbag/enum/orient_2d.h>

namespace bp = boost::polygon;

namespace cbag {
namespace layout {

class tech;

class grid_info {
  private:
    orient_2d dir = orient_2d::HORIZONTAL;
    offset_t w = 0;
    offset_t sp = 0;

  public:
};

class routing_grid {
  private:
    tech *tech_ptr = nullptr;

  public:
    routing_grid(tech *tech_ptr);
};

} // namespace layout
} // namespace cbag

#endif
