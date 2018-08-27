#ifndef CBAG_LAYOUT_VIA_H
#define CBAG_LAYOUT_VIA_H

#include <string>

#include <cbag/common/vector.h>
#include <cbag/layout/polygon_ref.h>

namespace cbag {
namespace layout {

class rectangle;

class via {
  public:
    inline via(std::string via_id, uint32_t num_row, uint32_t num_col, uint32_t cut_w,
               uint32_t cut_h, int32_t cut_spx, int32_t cut_spy, int32_t lay1_encx,
               int32_t lay1_ency, int32_t lay1_offx, int32_t lay1_offy, int32_t lay2_encx,
               int32_t lay2_ency, int32_t lay2_offx, int32_t lay2_offy)
        : via_id(std::move(via_id)), num_row(num_row), num_col(num_col), cut_w(cut_w), cut_h(cut_h),
          cut_spacing(cut_spx, cut_spy), lay1_enc(lay1_encx, lay1_ency),
          lay1_off(lay1_offx, lay1_offy), lay2_enc(lay2_encx, lay2_ency),
          lay2_off(lay2_offx, lay2_offy), lay1_ref(), lay2_ref() {}

  private:
    std::string via_id;
    uint32_t num_row, num_col;
    uint32_t cut_w, cut_h;
    vector cut_spacing;
    vector lay1_enc;
    vector lay1_off;
    vector lay2_enc;
    vector lay2_off;
    polygon_ref<rectangle> lay1_ref;
    polygon_ref<rectangle> lay2_ref;
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
