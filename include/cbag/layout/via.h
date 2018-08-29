#ifndef CBAG_LAYOUT_VIA_H
#define CBAG_LAYOUT_VIA_H

#include <string>

#include <cbag/common/vector.h>
#include <cbag/layout/polygon_ref.h>

namespace cbag {
namespace layout {

class rectangle;

class via {
  private:
    std::string via_id;
    uint32_t num_row, num_col;
    uint32_t cut_w, cut_h;
    vector cut_spacing;
    vector lay1_enc;
    vector lay1_off;
    vector lay2_enc;
    vector lay2_off;
    polygon_ref<rectangle> lay1_ref{};
    polygon_ref<rectangle> lay2_ref{};

  public:
    via(std::string via_id, uint32_t num_row, uint32_t num_col, uint32_t cut_w, uint32_t cut_h,
        int32_t cut_spx, int32_t cut_spy, int32_t lay1_encx, int32_t lay1_ency, int32_t lay1_offx,
        int32_t lay1_offy, int32_t lay2_encx, int32_t lay2_ency, int32_t lay2_offx,
        int32_t lay2_offy);
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
