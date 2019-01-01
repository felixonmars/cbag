
#ifndef CBAG_COMMON_VIA_INFO_H
#define CBAG_COMMON_VIA_INFO_H

#include <array>
#include <cmath>
#include <vector>

#include <cbag/common/typedefs.h>
#include <cbag/common/vector.h>
#include <cbag/enum/orient_2d.h>

namespace cbag {
namespace layout {

class via_param;

struct venc_data {
    offset_t width;
    std::vector<vector> enc_list;

    venc_data();
};

using venc_info = std::vector<venc_data>;

using via_cnt_t = std::tuple<cnt_t, std::array<cnt_t, 2>>;

class via_info {
  private:
    std::string cut_type;
    vector cut_dim = {0, 0};
    vector sp = {0, 0};
    std::vector<vector> sp2_list;
    std::vector<vector> sp3_list;
    std::array<venc_info, 2> enc_list;

  public:
    via_info();

    via_info(std::string &&ctype, vector &&cdim, vector &&s, std::vector<vector> &&s2_list,
             std::vector<vector> &&s3_list, std::array<venc_info, 2> &&e_list);

    via_param get_via_param(vector dim, orient_2d bot_dir, orient_2d top_dir, bool extend) const;
};

} // namespace layout
} // namespace cbag

#endif
