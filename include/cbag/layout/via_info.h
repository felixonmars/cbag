
#ifndef CBAG_COMMON_VIA_INFO_H
#define CBAG_COMMON_VIA_INFO_H

#include <array>
#include <cmath>
#include <vector>

#include <cbag/common/dim_t.h>
#include <cbag/common/typedefs.h>
#include <cbag/enum/orient_2d.h>

namespace cbag {
namespace layout {

struct venc_data {
    dist_t width;
    std::vector<dim_t> enc_list;

    venc_data();
};

using venc_info = std::vector<venc_data>;

using via_cnt_t = std::tuple<cnt_t, std::array<cnt_t, 2>>;

class via_info {
  private:
    std::string cut_type;
    dim_t cut_dim = {0, 0};
    dim_t sp = {0, 0};
    std::vector<dim_t> sp2_list;
    std::vector<dim_t> sp3_list;
    std::array<venc_info, 2> enc_list;

  public:
    via_info();

    via_info(std::string &&ctype, dim_t &&cdim, dim_t &&s, std::vector<dim_t> &&s2_list,
             std::vector<dim_t> &&s3_list, std::array<venc_info, 2> &&e_list);

    via_cnt_t get_num_via(dim_t dim, orient_2d bot_dir, orient_2d top_dir, bool extend) const;
};

} // namespace layout
} // namespace cbag

#endif
