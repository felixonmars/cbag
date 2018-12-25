#include <cbag/common/box_t_util.h>
#include <cbag/layout/cellview_util.h>
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

} // namespace layout
} // namespace cbag
