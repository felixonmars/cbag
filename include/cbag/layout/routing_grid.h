#ifndef CBAG_LAYOUT_ROUTING_GRID_H
#define CBAG_LAYOUT_ROUTING_GRID_H

#include <cbag/common/typedefs.h>
#include <cbag/enum/orient_2d.h>

namespace cbag {
namespace layout {

class tech;
class track_info;

class routing_grid {
  private:
    const tech *tech_ptr = nullptr;
    int bot_layer = 0;
    std::vector<track_info> info_list;

  public:
    routing_grid();

    routing_grid(const tech *t, const std::string &fname);

    bool operator==(const routing_grid &rhs) const noexcept;

    const tech *get_tech() const noexcept;

    orient_2d get_direction(int level) const;
};

} // namespace layout
} // namespace cbag

#endif
