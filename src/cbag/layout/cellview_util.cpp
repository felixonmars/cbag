#include <cstdlib>

#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation_util.h>
#include <cbag/layout/cellview_poly.h>
#include <cbag/layout/cellview_util.h>
#include <cbag/layout/cv_obj_ref.h>
#include <cbag/layout/grid_object.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/track_info_util.h>
#include <cbag/layout/via_param_util.h>
#include <cbag/layout/via_wrapper.h>
#include <cbag/layout/wire_width.h>

namespace cbag {
namespace layout {

box_t get_bbox(const cellview &cv, const std::string &layer, const std::string &purpose) {
    auto ans = box_t::get_invalid_box();
    // merge geometry bounding box
    auto iter = cv.find_geometry(layer_t_at(*cv.get_tech(), layer, purpose));
    if (iter != cv.end_geometry()) {
        merge(ans, iter->second.get_bbox());
    }
    // merge instance bounding box
    for (auto it = cv.begin_inst(); it != cv.end_inst(); ++it) {
        merge(ans, it->second.get_bbox(layer, purpose));
    }
    return ans;
}

void add_pin(cellview &cv, const std::string &layer, const std::string &net,
             const std::string &label, const box_t &bbox) {
    auto lay_id = layer_id_at(*cv.get_tech(), layer);
    cv.add_pin(lay_id, std::string(net), std::string(label), box_t(bbox));
}

void add_pin_arr(cellview &cv, const std::string &net, const std::string &label,
                 const wire_array &warr) {
    auto &tid = warr.get_track_id_ref();
    auto level = tid.get_level();
    auto cur_htr = tid.get_htr();
    auto ntr = tid.get_ntr();
    auto num = tid.get_num();
    auto pitch = tid.get_pitch();
    auto [lower, upper] = warr.get_coord();

    auto &tinfo = cv.get_grid()->track_info_at(level);
    auto winfo = tinfo.get_wire_width(ntr);
    auto winfo_end = winfo.end();
    auto tr_dir = tinfo.get_direction();
    auto htr_pitch = pitch * tinfo.get_pitch() / 2;
    for (decltype(num) idx = 0; idx < num; ++idx, cur_htr += htr_pitch) {
        auto [lay, purp] = get_layer_t(*cv.get_grid(), level, cur_htr);
        for (auto witer = winfo.begin(); witer != winfo_end; ++witer) {
            auto &[rel_htr, wire_w] = *witer;
            auto half_w = wire_w / 2;
            auto center = htr_to_coord(tinfo, cur_htr + rel_htr);
            cv.add_pin(lay, std::string(net), std::string(label),
                       box_t(tr_dir, lower, upper, center - half_w, center + half_w));
        }
    }
}

cv_obj_ref<via_wrapper> add_via(cellview &cv, transformation xform, std::string via_id,
                                const via_param &params, bool add_layers, bool commit) {
    return {&cv, via_wrapper(via(std::move(xform), std::move(via_id), params), add_layers), commit};
}

void add_via_arr(cellview &cv, const transformation &xform, const std::string &via_id,
                 const via_param &params, bool add_layers, std::array<cnt_t, 2> num_arr,
                 std::array<offset_t, 2> sp_arr) {
    offset_t dx = 0;
    for (cnt_t xidx = 0; xidx < num_arr[0]; ++xidx, dx += sp_arr[0]) {
        offset_t dy = 0;
        for (cnt_t yidx = 0; yidx < num_arr[1]; ++yidx, dy += sp_arr[1]) {
            cv.add_object(via_wrapper(via(get_move_by(xform, dx, dy), via_id, params), add_layers));
        }
    }
}

void add_via_on_intersection(cellview &cv, const wire_array &warr1, const wire_array &warr2,
                             bool extend, bool contain) {
    auto lev1 = warr1.get_track_id_ref().get_level();
    auto lev2 = warr2.get_track_id_ref().get_level();
    auto diff = lev1 - lev2;
    if (std::abs(diff) != 1)
        throw std::invalid_argument(
            fmt::format("Cannot create via between layers {} and {}", lev1, lev2));

    std::array<const wire_array *, 2> warr_arr;
    auto idx1 = static_cast<int>(diff > 0);
    warr_arr[idx1] = &warr1;
    warr_arr[1 - idx1] = &warr2;
    auto &grid = *cv.get_grid();
    auto &tech = *grid.get_tech();
    auto &tid0 = warr_arr[0]->get_track_id_ref();
    auto &tid1 = warr_arr[1]->get_track_id_ref();
    auto &tinfo0 = grid.track_info_at(tid0.get_level());
    auto &tinfo1 = grid.track_info_at(tid1.get_level());
    auto dir0 = tinfo0.get_direction();
    auto dir1 = tinfo1.get_direction();
    if (dir0 == dir1)
        throw std::invalid_argument("Cannot draw vias between layers with same direction.");

    for (auto i0 = warr_arr[0]->begin_rect(grid), s0 = warr_arr[0]->end_rect(grid); i0 != s0;
         ++i0) {
        auto [lay0, box0] = *i0;
        for (auto i1 = warr_arr[1]->begin_rect(grid), s1 = warr_arr[1]->end_rect(grid); i1 != s1;
             ++i1) {
            auto [lay1, box1] = *i1;
            auto via_box = get_intersect(box0, box1);
            if (is_physical(via_box)) {
                auto box_dim = dim(via_box);
                auto &via_id = tech.get_via_id(direction::LOWER, lay0, lay1);
                auto via_param =
                    tech.get_via_param(box_dim, via_id, direction::LOWER, dir0, dir1, extend);
                if (!empty(via_param)) {
                    if (contain) {
                        auto via_ext =
                            get_via_extensions(via_param, box_dim, direction::LOWER, dir0, dir1);
                        // check that via extensions are contained inside the wire
                        if (lower(box0, dir0) > (lower(via_box, dir0) - via_ext[0]) ||
                            upper(box0, dir0) < (upper(via_box, dir0) + via_ext[0]) ||
                            lower(box1, dir1) > (lower(via_box, dir1) - via_ext[1]) ||
                            upper(box1, dir1) < (upper(via_box, dir1) + via_ext[1])) {
                            continue;
                        }
                    }
                    // we can draw via safely
                    cv.add_object(via_wrapper(
                        via(make_xform(xm(via_box), ym(via_box)), via_id, via_param), false));
                }
            }
        }
    }
}

std::array<std::array<offset_t, 2>, 2>
connect_box_track(cellview &cv, direction vdir, layer_t key, const box_t &box,
                  std::array<cnt_t, 2> num, std::array<offset_t, 2> sp, const track_id &tid,
                  const std::array<std::optional<offset_t>, 2> &box_ext,
                  const std::array<std::optional<offset_t>, 2> &tr_ext, min_len_mode mode) {
    std::array<std::array<offset_t, 2>, 2> ans;

    auto &grid = *cv.get_grid();
    auto &tech = *grid.get_tech();
    auto &tinfo = grid.track_info_at(tid.get_level());
    auto tr_dir = tinfo.get_direction();
    auto p_dir = perpendicular(tr_dir);
    auto tr_didx = to_int(tr_dir);
    auto p_didx = 1 - tr_didx;

    auto v_didx = to_int(vdir);
    auto tv_didx = 1 - v_didx;
    auto num_box = num[tr_didx];
    auto sp_box = sp[tr_didx];
    decltype(sp_box) delta = sp_box * (num_box - 1);
    decltype(sp_box) p_delta = sp[p_didx] * (num[p_didx] - 1);
    ans[v_didx] = box.intvs[p_didx];
    ans[v_didx][p_delta > 0] += p_delta;
    ans[tv_didx] = box.intvs[tr_didx];
    ans[tv_didx][delta > 0] += delta;

    for (auto iter = begin_rect(grid, tid, ans[1]), stop = end_rect(grid, tid, ans[1]);
         iter != stop; ++iter) {
        // compute via
        auto [tr_key, tr_box] = *iter;
        auto &via_id = tech.get_via_id(vdir, key, tr_key);
        auto via_box = box_t(tr_dir, box.intvs[tr_didx][0], box.intvs[tr_didx][1],
                             tr_box.intvs[p_didx][0], tr_box.intvs[p_didx][1]);
        auto via_dim = dim(via_box);
        auto via_param = tech.get_via_param(via_dim, via_id, vdir, p_dir, tr_dir, true);

        // add via
        auto xform = make_xform(xm(via_box), ym(via_box));
        auto via_num = std::array<cnt_t, 2>{1, 1};
        auto via_sp = std::array<offset_t, 2>{0, 0};
        via_num[tr_didx] = num_box;
        via_sp[tr_didx] = sp_box;
        add_via_arr(cv, xform, via_id, via_param, false, via_num, via_sp);

        // get via coordinate boundaries
        auto via_ext = get_via_extensions(via_param, via_dim, vdir, p_dir, tr_dir);
        ans[v_didx][0] = std::min(ans[v_didx][0], via_box.intvs[p_didx][0] - via_ext[p_didx]);
        ans[v_didx][1] = std::max(ans[v_didx][1], via_box.intvs[p_didx][1] + via_ext[p_didx]);
        ans[tv_didx][0] = std::min(ans[tv_didx][0], via_box.intvs[tr_didx][0] - via_ext[tr_didx]);
        ans[tv_didx][1] = std::max(ans[tv_didx][1], via_box.intvs[tr_didx][1] + via_ext[tr_didx]);
    }

    // perform optional extensions
    if (box_ext[0])
        ans[v_didx][0] = std::min(ans[v_didx][0], *box_ext[0]);
    if (box_ext[1])
        ans[v_didx][1] = std::max(ans[v_didx][1], *box_ext[1]);
    if (tr_ext[0])
        ans[tv_didx][0] = std::min(ans[tv_didx][0], *tr_ext[0]);
    if (tr_ext[1])
        ans[tv_didx][1] = std::max(ans[tv_didx][1], *tr_ext[1]);

    // perform minimum length correction
    auto tr_len = ans[tv_didx][1] - ans[tv_didx][0];
    switch (mode) {
    case min_len_mode::LOWER:
        tr_len = std::max(tr_len, get_min_length(tech, tid.get_level(),
                                                 tinfo.get_wire_width(tid.get_ntr()), false));
        ans[tv_didx][0] = ans[tv_didx][1] - tr_len;
        break;
    case min_len_mode::UPPER:
        tr_len = std::max(tr_len, get_min_length(tech, tid.get_level(),
                                                 tinfo.get_wire_width(tid.get_ntr()), false));
        ans[tv_didx][1] = ans[tv_didx][0] + tr_len;
        break;
    case min_len_mode::MIDDLE:
        tr_len = std::max(tr_len, get_min_length(tech, tid.get_level(),
                                                 tinfo.get_wire_width(tid.get_ntr()), true));
        ans[tv_didx][0] = (ans[tv_didx][0] + ans[tv_didx][1] - tr_len) / 2;
        ans[tv_didx][1] = ans[tv_didx][0] + tr_len;
        break;
    default:
        break;
    }

    // add bbox wires
    auto new_box =
        box_t(tr_dir, box.intvs[tr_didx][0], box.intvs[tr_didx][1], ans[v_didx][0], ans[v_didx][1]);
    auto new_num_box = std::array<cnt_t, 2>{1, 1};
    auto new_sp_box = std::array<offset_t, 2>{0, 0};
    new_num_box[tr_didx] = num_box;
    new_sp_box[tr_didx] = sp_box;
    add_rect_arr(cv, key, new_box, new_num_box, new_sp_box);

    // add track wires and return track coordinates
    add_warr(cv, tid, ans[tv_didx]);
    return ans;
}

std::array<std::array<offset_t, 2>, 2>
connect_warr_track(cellview &cv, const wire_array &warr, const track_id &tid,
                   const std::array<std::optional<offset_t>, 2> &box_ext,
                   const std::array<std::optional<offset_t>, 2> &tr_ext) {}

void add_label(cellview &cv, const std::string &layer, const std::string &purpose,
               transformation xform, std::string label) {
    cv.add_label(layer_t_at(*cv.get_tech(), layer, purpose), std::move(xform), std::move(label));
}

cv_obj_ref<instance> add_prim_instance(cellview &cv, std::string lib, std::string cell,
                                       std::string view, std::string name,
                                       cbag::transformation xform, cnt_t nx, cnt_t ny, offset_t spx,
                                       offset_t spy, bool commit) {
    return {&cv,
            instance(std::move(name), std::move(lib), std::move(cell), std::move(view),
                     std::move(xform), nx, ny, spx, spy),
            commit};
}

cv_obj_ref<instance> add_instance(cellview &cv, const cellview *master, std::string name,
                                  cbag::transformation xform, cnt_t nx, cnt_t ny, offset_t spx,
                                  offset_t spy, bool commit) {
    return {&cv, instance(std::move(name), master, std::move(xform), nx, ny, spx, spy), commit};
}

} // namespace layout
} // namespace cbag
