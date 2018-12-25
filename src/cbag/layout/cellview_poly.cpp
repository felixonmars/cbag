#include <cbag/common/box_t_util.h>
#include <cbag/layout/cellview_poly.h>
#include <cbag/layout/tech_util.h>

namespace cbag {
namespace layout {

shape_ref<box_t> add_rect(cellview &cv, const std::string &layer, const std::string &purpose,
                          bool is_horiz, box_t bbox, bool commit) {
    return {&cv, get_layer_t(*(cv.get_tech()), layer, purpose), is_horiz, std::move(bbox), commit};
}

void add_rect_arr(cellview &cv, const std::string &layer, const std::string &purpose,
                  const box_t &box, bool is_horiz, cnt_t nx, cnt_t ny, offset_t spx, offset_t spy) {
    auto &geo = cv.make_geometry(get_layer_t(*cv.get_tech(), layer, purpose));
    offset_t dx = 0;
    for (decltype(nx) xidx = 0; xidx < nx; ++xidx, dx += spx) {
        offset_t dy = 0;
        for (decltype(ny) yidx = 0; yidx < ny; ++yidx, dy += spy) {
            geo.add_shape(get_move_by(box, dx, dy), is_horiz);
        }
    }
}

} // namespace layout
} // namespace cbag
