#include <algorithm>

#include <boost/functional/hash.hpp>

#include <cbag/layout/via_info.h>
#include <cbag/layout/via_param.h>
#include <cbag/util/unique_heap.h>

namespace cbag {
namespace layout {

constexpr offset_t MAX_INT = std::numeric_limits<offset_t>::max();

venc_data::venc_data() = default;

via_info::via_info() = default;

via_info::via_info(std::string &&ctype, vector &&cdim, vector &&s, std::vector<vector> &&s2_list,
                   std::vector<vector> &&s3_list, std::array<venc_info, 2> &&e_list)
    : cut_type(std::move(ctype)), cut_dim(std::move(cdim)), sp(std::move(s)),
      sp2_list(std::move(s2_list)), sp3_list(std::move(s3_list)), enc_list(std::move(e_list)) {
    if (sp2_list.empty()) {
        sp2_list.emplace_back(sp);
    }
    if (sp3_list.empty()) {
        sp3_list.insert(sp3_list.end(), sp2_list.begin(), sp2_list.end());
    }
}

cnt_t get_n_max(offset_t dim, offset_t w, offset_t sp) {
    if (sp == MAX_INT) {
        return (dim > w) ? 1 : 0;
    }
    return (dim + sp) / (w + sp);
}

const std::vector<vector> &get_enc_list(const venc_info &enc_info, offset_t width) {
    for (const auto &data : enc_info) {
        if (width <= data.width)
            return data.enc_list;
    }
    return enc_info.back().enc_list;
}

vector get_metal_dim(vector box_dim, vector arr_dim, const venc_info &enc_info, orient_2d_t dir_idx,
                     bool extend) {
    auto pdir_idx = 1 - dir_idx;
    auto wire_w = box_dim[pdir_idx];
    vector ans;
    ans[dir_idx] = MAX_INT;
    ans[pdir_idx] = wire_w;
    for (const auto &enc_vec : get_enc_list(enc_info, wire_w)) {
        vector enc_dim = {2 * enc_vec[0] + arr_dim[0], 2 * enc_vec[1] + arr_dim[1]};
        if (enc_dim[pdir_idx] <= box_dim[pdir_idx]) {
            if (extend || enc_dim[dir_idx] <= box_dim[dir_idx]) {
                // enclosure rule passed, get optimal metal dimension
                ans[dir_idx] = std::min(ans[dir_idx], enc_dim[dir_idx]);
            }
        }
    }
    if (ans[dir_idx] == MAX_INT) {
        // no solution
        ans[0] = 0;
        ans[1] = 0;
    } else {
        ans[dir_idx] = std::max(ans[dir_idx], box_dim[dir_idx]);
    }

    return ans;
}

offset_t get_arr_dim(cnt_t n, offset_t w, offset_t sp) {
    if (sp == MAX_INT)
        if (n != 1)
            return 0;
        else
            return w;
    else
        return n * (w + sp) - sp;
}

via_param via_info::get_via_param(vector box_dim, orient_2d bot_dir, orient_2d top_dir,
                                  bool extend) const {
    via_param ans;

    auto bd_idx = static_cast<orient_2d_t>(bot_dir);
    auto td_idx = static_cast<orient_2d_t>(top_dir);

    // get maximum possible number of vias
    vector min_sp = sp;
    for (const auto &arr : sp2_list) {
        min_sp[0] = std::min(min_sp[0], arr[0]);
        min_sp[1] = std::min(min_sp[1], arr[1]);
    }
    for (const auto &arr : sp3_list) {
        min_sp[0] = std::min(min_sp[0], arr[0]);
        min_sp[1] = std::min(min_sp[1], arr[1]);
    }
    auto nx_max = get_n_max(box_dim[0], cut_dim[0], min_sp[0]);
    auto ny_max = get_n_max(box_dim[1], cut_dim[1], min_sp[1]);

    // check if it's possible to palce a via
    if (nx_max == 0 || ny_max == 0)
        return ans;

    // use priority queue to find maximum number of vias
    util::unique_heap<via_cnt_t, boost::hash<via_cnt_t>> heap;
    heap.emplace(nx_max * ny_max, std::array<cnt_t, 2>{nx_max, ny_max});

    std::vector<vector> sp1_list({sp});

    while (!heap.empty()) {
        auto &[via_cnt, num_via] = heap.top();

        // get list of valid via spacing
        const std::vector<vector> *sp_vec_ptr;
        if (num_via[0] == 2 && num_via[1] == 2) {
            sp_vec_ptr = &sp2_list;
        } else if (num_via[0] > 1 && num_via[1] > 1) {
            sp_vec_ptr = &sp3_list;
        } else {
            sp_vec_ptr = &sp1_list;
        }

        // find optimal legal enclosure via
        vector opt_arr_dim;
        std::array<vector, 2> opt_mdim = {vector{MAX_INT, MAX_INT}, vector{MAX_INT, MAX_INT}};
        vector opt_sp = {-1, -1};
        for (const auto &sp_via : *sp_vec_ptr) {
            vector arr_dim = {get_arr_dim(num_via[0], cut_dim[0], sp_via[0]),
                              get_arr_dim(num_via[1], cut_dim[1], sp_via[1])};
            if (arr_dim[0] == 0 || arr_dim[1] == 0)
                continue;

            auto bot_dim = get_metal_dim(box_dim, arr_dim, enc_list[0], bd_idx, extend);
            if (bot_dim[0] == 0)
                continue;
            auto top_dim = get_metal_dim(box_dim, arr_dim, enc_list[1], td_idx, extend);
            if (top_dim[0] == 0)
                continue;

            if (bot_dim[bd_idx] <= opt_mdim[0][bd_idx] && top_dim[td_idx] <= opt_mdim[1][td_idx]) {
                opt_arr_dim = arr_dim;
                opt_mdim[0] = bot_dim;
                opt_mdim[1] = top_dim;
                opt_sp = sp_via;
            }
        }

        if (opt_sp[0] < 0) {
            // fail to find legal via enclosure
            // continue searching
            if (nx_max > 1)
                heap.emplace((nx_max - 1) * ny_max, std::array<cnt_t, 2>{nx_max - 1, ny_max});
            if (ny_max > 1)
                heap.emplace(nx_max * (ny_max - 1), std::array<cnt_t, 2>{nx_max, ny_max - 1});
        } else {
            // solution found
            // set space to 0 if ony 1 via, this prevents space with MAX_INT values.
            if (num_via[0] == 1)
                opt_sp[0] = 0;
            if (num_via[1] == 1)
                opt_sp[1] = 0;
            auto enc1x = (opt_mdim[0][0] - opt_arr_dim[0]) / 2;
            auto enc1y = (opt_mdim[0][1] - opt_arr_dim[1]) / 2;
            auto enc2x = (opt_mdim[1][0] - opt_arr_dim[0]) / 2;
            auto enc2y = (opt_mdim[1][1] - opt_arr_dim[1]) / 2;

            ans = {num_via[0], num_via[1], cut_dim[0], cut_dim[1], opt_sp[0], opt_sp[1], enc1x,
                   enc1x,      enc1y,      enc1y,      enc2x,      enc2x,     enc2y,     enc2y};
            return ans;
        }

        heap.pop();
    }

    return ans;
}

} // namespace layout
} // namespace cbag
