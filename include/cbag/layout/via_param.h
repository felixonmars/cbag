
#ifndef CBAG_COMMON_VIA_PARAM_H
#define CBAG_COMMON_VIA_PARAM_H

#include <cbag/common/typedefs.h>
#include <cbag/common/vector.h>

namespace cbag {

class via_param {
  public:
    cnt_t num[2] = {1, 1};
    dist_t cut_dim[2] = {0, 0};
    vector cut_spacing;
    vector lay1_enc;
    vector lay1_off;
    vector lay2_enc;
    vector lay2_off;

  public:
    via_param();

    via_param(cnt_t vnx, cnt_t vny, dist_t w, dist_t h, offset_t vspx, offset_t vspy,
              offset_t enc1l, offset_t enc1r, offset_t enc1t, offset_t enc1b, offset_t enc2l,
              offset_t enc2r, offset_t enc2t, offset_t enc2b);
};

} // namespace cbag

#endif
