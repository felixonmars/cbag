#include <algorithm>

#include <boost/functional/hash.hpp>

#include <cbag/layout/via_info.h>
#include <cbag/util/unique_heap.h>

namespace cbag {
namespace layout {

using via_cnt_t = std::tuple<cnt_t, cnt_t, cnt_t>;

venc_data::venc_data() = default;

via_info::via_info() = default;

via_info::via_info(std::string &&ctype, dim_t &&cdim, dim_t &&s, std::vector<dim_t> &&s2_list,
                   std::vector<dim_t> &&s3_list, std::array<venc_info, 2> &&e_list)
    : cut_type(std::move(ctype)), cut_dim(std::move(cdim)), sp(std::move(s)),
      sp2_list(std::move(s2_list)), sp3_list(std::move(s3_list)), enc_list(std::move(e_list)) {
    if (sp2_list.empty()) {
        sp2_list.emplace_back(sp);
    }
    if (sp3_list.empty()) {
        sp3_list.insert(sp3_list.end(), sp2_list.begin(), sp2_list.end());
    }
}

std::array<cnt_t, 2> via_info::get_num_via(dim_t dim) const {
    dim_t min_sp = sp;
    for (const auto &arr : sp2_list) {
        min_sp[0] = std::min(min_sp[0], arr[0]);
        min_sp[1] = std::min(min_sp[1], arr[1]);
    }
    for (const auto &arr : sp3_list) {
        min_sp[0] = std::min(min_sp[0], arr[0]);
        min_sp[1] = std::min(min_sp[1], arr[1]);
    }

    util::unique_heap<via_cnt_t, boost::hash<via_cnt_t>> heap;

    return {0, 0};
}

} // namespace layout
} // namespace cbag
