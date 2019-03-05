#include <cbag/common/box_t_util.h>
#include <cbag/layout/cellview_poly.h>
#include <cbag/layout/grid_object.h>
#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/track_info.h>

namespace cbag {
namespace layout {

shape_ref<box_t> add_rect(const std::shared_ptr<cellview> &cv_ptr, const std::string &layer,
                          const std::string &purpose, box_t bbox, bool commit) {

    return {cv_ptr, layer_t_at(*(cv_ptr->get_tech()), layer, purpose), std::move(bbox), commit};
}

void add_rect_arr(cellview &cv, layer_t &key, const box_t &box, std::array<cnt_t, 2> num,
                  std::array<offset_t, 2> sp) {
    box_t box_copy(box);
    for (cnt_t xidx = 0; xidx < num[0]; ++xidx, move_by(box_copy, sp[0], 0)) {
        auto tot_dy = num[1] * sp[1];
        for (cnt_t yidx = 0; yidx < num[1]; ++yidx, move_by(box_copy, 0, sp[1])) {
            cv.add_shape(key, box_copy);
        }
        move_by(box_copy, 0, -tot_dy);
    }
}

void add_rect_arr(cellview &cv, const std::string &layer, const std::string &purpose,
                  const box_t &box, cnt_t nx, cnt_t ny, offset_t spx, offset_t spy) {
    auto key = layer_t_at(*(cv.get_tech()), layer, purpose);
    add_rect_arr(cv, key, box, {nx, ny}, {spx, spy});
}

void add_warr(cellview &cv, const wire_array &warr) {
    cv.add_warr(warr.get_track_id_ref(), warr.get_coord());
}

} // namespace layout
} // namespace cbag
