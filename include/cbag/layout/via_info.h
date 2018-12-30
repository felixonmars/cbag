
#ifndef CBAG_COMMON_VIA_INFO_H
#define CBAG_COMMON_VIA_INFO_H

#include <array>
#include <cmath>
#include <vector>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

struct venc_data {
    dist_t width;
    std::vector<std::array<dist_t, 2>> enc_list;

    venc_data();
};

using venc_info = std::vector<venc_data>;

class via_info {
  private:
    std::string cut_type;
    std::array<dist_t, 2> cut_dim = {0, 0};
    std::array<dist_t, 2> sp = {0, 0};
    std::vector<std::array<dist_t, 2>> sp2_list;
    std::vector<std::array<dist_t, 2>> sp3_list;
    std::array<venc_info, 2> enc_list;

  public:
    via_info();

    via_info(std::string &&cut_type, std::array<dist_t, 2> &&cut_dim, std::array<dist_t, 2> &&sp,
             std::vector<std::array<dist_t, 2>> &&sp2_list,
             std::vector<std::array<dist_t, 2>> &&sp3_list, std::array<venc_info, 2> &&enc_list);
};

} // namespace layout
} // namespace cbag

#endif
