
#ifndef CBAG_COMMON_VIA_PARAM_H
#define CBAG_COMMON_VIA_PARAM_H

#include <cbag/common/typedefs.h>
#include <cbag/common/vector.h>

namespace cbag {

class via_param {
  public:
    uint32_t num[2] = {1, 1};
    dist_t cut_dim[2] = {0, 0};
    vector cut_spacing;
    vector lay1_enc;
    vector lay1_off;
    vector lay2_enc;
    vector lay2_off;

  public:
    via_param();

    via_param(uint32_t vnx, uint32_t vny, dist_t w, dist_t h, offset_t vspx, offset_t vspy,
              offset_t enc1x, offset_t enc1y, offset_t off1x, offset_t off1y, offset_t enc2x,
              offset_t enc2y, offset_t off2x, offset_t off2y);
};

} // namespace cbag

#endif
