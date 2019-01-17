#include <cbag/common/box_t_util.h>
#include <cbag/layout/cellview_poly.h>
#include <cbag/layout/grid_object.h>
#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/track_info.h>

namespace cbag {
namespace layout {

shape_ref<box_t> add_rect(cellview &cv, const std::string &layer, const std::string &purpose,
                          box_t bbox, bool commit) {

    return {&cv, layer_t_at(*(cv.get_tech()), layer, purpose), std::move(bbox), commit};
}

void add_rect_arr(cellview &cv, layer_t &key, const box_t &box, std::array<cnt_t, 2> num,
                  std::array<offset_t, 2> sp) {
    auto [marx, mary] = get_margins(*(cv.get_grid()), key, box);

    auto &geo = cv.make_geometry(key);
    offset_t dx = 0;
    for (cnt_t xidx = 0; xidx < num[0]; ++xidx, dx += sp[0]) {
        offset_t dy = 0;
        for (cnt_t yidx = 0; yidx < num[1]; ++yidx, dy += sp[1]) {
            geo.add_shape(get_move_by(box, dx, dy), marx, mary);
        }
    }
}

void add_rect_arr(cellview &cv, const std::string &layer, const std::string &purpose,
                  const box_t &box, cnt_t nx, cnt_t ny, offset_t spx, offset_t spy) {
    auto key = layer_t_at(*(cv.get_tech()), layer, purpose);
    add_rect_arr(cv, key, box, {nx, ny}, {spx, spy});
}

void add_warr(cellview &cv, const wire_array &warr) {
    add_warr(cv, warr.get_track_id_ref(), warr.get_coord());
}

void add_warr(cellview &cv, const track_id &tid, std::array<offset_t, 2> coord) {
    auto grid = *cv.get_grid();
    for (auto iter = begin_rect(grid, tid, coord), stop = end_rect(grid, tid, coord); iter != stop;
         ++iter) {
        auto [key, box] = *iter;
        auto &geo = cv.make_geometry(key);
        auto [spx, spy] = get_margins(grid, key, box);
        geo.add_shape(box, spx, spy);
    }
}

} // namespace layout
} // namespace cbag
