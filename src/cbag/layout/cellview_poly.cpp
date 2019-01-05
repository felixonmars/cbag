#include <cbag/common/box_t_util.h>
#include <cbag/layout/cellview_poly.h>
#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech_util.h>

namespace cbag {
namespace layout {

shape_ref<box_t> add_rect(cellview &cv, const std::string &layer, const std::string &purpose,
                          box_t bbox, bool commit) {

    return {&cv, layer_t_at(*(cv.get_tech()), layer, purpose), std::move(bbox), commit};
}

void add_rect_arr(cellview &cv, const std::string &layer, const std::string &purpose,
                  const box_t &box, cnt_t nx, cnt_t ny, offset_t spx, offset_t spy) {
    auto lay = layer_t_at(*(cv.get_tech()), layer, purpose);
    auto [marx, mary] = get_margins(*(cv.get_grid()), lay, box);

    auto &geo = cv.make_geometry(lay);
    offset_t dx = 0;
    for (decltype(nx) xidx = 0; xidx < nx; ++xidx, dx += spx) {
        offset_t dy = 0;
        for (decltype(ny) yidx = 0; yidx < ny; ++yidx, dy += spy) {
            geo.add_shape(get_move_by(box, dx, dy), marx, mary);
        }
    }
}

} // namespace layout
} // namespace cbag
