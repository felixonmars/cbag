
#ifndef CBAG_COMMON_VIA_INFO_H
#define CBAG_COMMON_VIA_INFO_H

#include <array>
#include <cmath>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

class via_info {
  public:
    std::string cut_type;
    std::array<dist_t, 2> cut_dim = {0, 0};
    double_t via_scale;

    via_info();

    via_info(std::string cut_type, dist_t cut_w, dist_t cut_h, double_t scale);
};

} // namespace layout
} // namespace cbag

#endif
