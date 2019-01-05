#ifndef CBAG_LAYOUT_ROUTING_GRID_H
#define CBAG_LAYOUT_ROUTING_GRID_H

#include <cbag/common/typedefs.h>
#include <cbag/enum/orient_2d.h>

namespace bp = boost::polygon;

namespace cbag {
namespace layout {

class tech;
class track_info;

class routing_grid {
  private:
    const tech *tech_ptr = nullptr;
    cnt_t bot_layer = 0;
    std::vector<track_info> info_list;

  public:
    routing_grid();

    routing_grid(const tech *t, const std::string &fname);
};

} // namespace layout
} // namespace cbag

#endif
