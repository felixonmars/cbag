#ifndef CBAG_LAYOUT_ROUTING_GRID_H
#define CBAG_LAYOUT_ROUTING_GRID_H

#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>

namespace bp = boost::polygon;

namespace cbag {
namespace layout {

class tech;

class track_config {
  private:
    bp::orientation_2d_enum orient;
    dist_t width;
    dist_t space;
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
