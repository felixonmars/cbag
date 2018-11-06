
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
    via_param() = default;
};

} // namespace cbag

#endif
