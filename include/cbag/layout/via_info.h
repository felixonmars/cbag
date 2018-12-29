
#ifndef CBAG_COMMON_VIA_INFO_H
#define CBAG_COMMON_VIA_INFO_H

#include <array>
#include <cmath>
#include <vector>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

using venc_list_t = std::vector<std::pair<dist_t, std::vector<std::array<dist_t, 2>>>>;

class via_info {
  private:
    std::string cut_type;
    std::array<dist_t, 2> cut_dim = {0, 0};
    std::array<dist_t, 2> sp = {0, 0};
    std::vector<std::array<dist_t, 2>> sp2_list;
    std::vector<std::array<dist_t, 2>> sp3_list;
    std::array<venc_list_t, 2> enc_list;
    double_t via_scale;

  public:
    via_info();
};

} // namespace layout
} // namespace cbag

#endif
