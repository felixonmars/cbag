#ifndef CBAG_LAYOUT_ROUTING_GRID_H
#define CBAG_LAYOUT_ROUTING_GRID_H

#include <cbag/util/interval.h>

#include <cbag/common/typedefs.h>
#include <cbag/enum/orient_2d.h>

namespace bp = boost::polygon;

namespace cbag {
namespace layout {

class tech;

class track_info {
  private:
    orient_2d dir = orient_2d::HORIZONTAL;
    offset_t w = 0;
    offset_t sp = 0;
    cbag::util::disjoint_intvs<> w_intvs;

  public:
    track_info();

    track_info(orient_2d tr_dir, offset_t tr_w, offset_t tr_sp,
               const std::vector<std::array<offset_t, 2>> &intv_list);
};

class routing_grid {
  private:
    tech *tech_ptr = nullptr;
    cnt_t bot_layer = 0;
    std::vector<track_info> info_list;

  public:
    routing_grid();
};

} // namespace layout
} // namespace cbag

#endif
