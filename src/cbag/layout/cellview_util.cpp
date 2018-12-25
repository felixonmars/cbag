#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation_util.h>
#include <cbag/layout/cellview_util.h>
#include <cbag/layout/cv_obj_ref.h>
#include <cbag/layout/tech_util.h>

namespace cbag {
namespace layout {

box_t get_bbox(const cellview &cv, const std::string &layer, const std::string &purpose) {
    auto ans = box_t::get_invalid_box();
    // merge geometry bounding box
    auto iter = cv.find_geometry(get_layer_t(*cv.get_tech(), layer, purpose));
    if (iter != cv.end_geometry()) {
        merge(ans, iter->second.get_bbox());
    }
    // merge instance bounding box
    for (auto it = cv.begin_inst(); it != cv.end_inst(); ++it) {
        merge(ans, it->second.get_bbox(layer, purpose));
    }
    return ans;
}

cv_obj_ref<via> add_via(cellview &cv, transformation xform, std::string via_id, bool add_layers,
                        bool bot_horiz, bool top_horiz, cnt_t vnx, cnt_t vny, dist_t w, dist_t h,
                        offset_t vspx, offset_t vspy, offset_t enc1l, offset_t enc1r,
                        offset_t enc1t, offset_t enc1b, offset_t enc2l, offset_t enc2r,
                        offset_t enc2t, offset_t enc2b, bool commit) {
    return {&cv,
            via(std::move(xform), std::move(via_id),
                via_param(vnx, vny, w, h, vspx, vspy, enc1l, enc1r, enc1t, enc1b, enc2l, enc2r,
                          enc2t, enc2b),
                add_layers, bot_horiz, top_horiz),
            commit};
}

void add_via_arr(cellview &cv, const transformation &xform, const std::string &via_id,
                 bool add_layers, bool bot_horiz, bool top_horiz, cnt_t vnx, cnt_t vny, dist_t w,
                 dist_t h, offset_t vspx, offset_t vspy, offset_t enc1l, offset_t enc1r,
                 offset_t enc1t, offset_t enc1b, offset_t enc2l, offset_t enc2r, offset_t enc2t,
                 offset_t enc2b, cnt_t nx, cnt_t ny, offset_t spx, offset_t spy) {
    via_param param{vnx,   vny,   w,     h,     vspx,  vspy,  enc1l,
                    enc1r, enc1t, enc1b, enc2l, enc2r, enc2t, enc2b};

    offset_t dx = 0;
    for (decltype(nx) xidx = 0; xidx < nx; ++xidx, dx += spx) {
        offset_t dy = 0;
        for (decltype(ny) yidx = 0; yidx < ny; ++yidx, dy += spy) {
            cv.add_object(
                via(get_move_by(xform, dx, dy), via_id, param, add_layers, bot_horiz, top_horiz));
        }
    }
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
