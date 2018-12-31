#include <algorithm>

#include <boost/functional/hash.hpp>

#include <cbag/layout/via_info.h>
#include <cbag/util/unique_heap.h>

namespace cbag {
namespace layout {

constexpr dist_t MAX_SP = std::numeric_limits<dist_t>::max();

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

cnt_t get_n_max(dist_t dim, dist_t w, dist_t sp) {
    if (sp == MAX_SP) {
        return (dim > w) ? 1 : 0;
    }
    return (dim + sp) / (w + sp);
}

dist_t get_arr_dim(cnt_t n, dist_t w, dist_t sp) {
    if (sp == MAX_SP)
        if (n != 1)
            return 0;
        else
            return w;
    else
        return n * (w + sp) - sp;
}

via_cnt_t via_info::get_num_via(dim_t dim, orient_2d bot_dir, orient_2d top_dir,
                                bool extend) const {
    dim_t min_sp = sp;
    for (const auto &arr : sp2_list) {
        min_sp[0] = std::min(min_sp[0], arr[0]);
        min_sp[1] = std::min(min_sp[1], arr[1]);
    }
    for (const auto &arr : sp3_list) {
        min_sp[0] = std::min(min_sp[0], arr[0]);
        min_sp[1] = std::min(min_sp[1], arr[1]);
    }

    auto nx_max = get_n_max(dim[0], cut_dim[0], min_sp[0]);
    auto ny_max = get_n_max(dim[1], cut_dim[1], min_sp[1]);
    if (nx_max == 0 || ny_max == 0)
        return {0, std::array<cnt_t, 2>{0, 0}};

    util::unique_heap<via_cnt_t, boost::hash<via_cnt_t>> heap;
    heap.emplace(nx_max * ny_max, std::array<cnt_t, 2>{nx_max, ny_max});

    std::vector<dim_t> sp1_list({sp});

    while (!heap.empty()) {
        auto &[via_cnt, num_via] = heap.top();

        const std::vector<dim_t> *sp_vec_ptr;
        if (num_via[0] == 2 && num_via[1] == 2) {
            sp_vec_ptr = &sp2_list;
        } else if (num_via[0] > 1 && num_via[1] > 1) {
            sp_vec_ptr = &sp3_list;
        } else {
            sp_vec_ptr = &sp1_list;
        }

        for (const auto &sp_via : *sp_vec_ptr) {
            std::array<dist_t, 2> arr_dim = {get_arr_dim(num_via[0], cut_dim[0], sp_via[0]),
                                             get_arr_dim(num_via[1], cut_dim[1], sp_via[1])};
            if (arr_dim[0] == 0 || arr_dim[1] == 0)
                continue;
        }

        heap.pop();
    }

    return {0, std::array<cnt_t, 2>{0, 0}};
}

} // namespace layout
} // namespace cbag
