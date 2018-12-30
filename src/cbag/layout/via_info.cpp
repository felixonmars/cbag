#include <cbag/layout/via_info.h>

namespace cbag {
namespace layout {

venc_data::venc_data() = default;

via_info::via_info() = default;

via_info::via_info(std::string &&cut_type, std::array<dist_t, 2> &&cut_dim,
                   std::array<dist_t, 2> &&sp, std::vector<std::array<dist_t, 2>> &&sp2_list,
                   std::vector<std::array<dist_t, 2>> &&sp3_list,
                   std::array<venc_info, 2> &&enc_list)
    : cut_type(std::move(cut_type)), cut_dim(std::move(cut_dim)), sp(std::move(sp)),
      sp2_list(std::move(sp2_list)), sp3_list(std::move(sp3_list)), enc_list(std::move(enc_list)) {}

} // namespace layout
} // namespace cbag
