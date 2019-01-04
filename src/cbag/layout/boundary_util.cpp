#include <array>

#include <cbag/layout/boundary_util.h>
#include <cbag/layout/pt_traits.h>

namespace cbag {
namespace layout {

boundary from_box(boundary_type type, const box_t &box) {
    boundary ans{type};

    std::array<point, 4> pts = {
        point{box.intvs[0][0], box.intvs[1][0]}, point{box.intvs[0][0], box.intvs[1][1]},
        point{box.intvs[0][1], box.intvs[1][1]}, point{box.intvs[0][1], box.intvs[1][0]}};
    ans.set(pts.begin(), pts.end());

    return ans;
}

} // namespace layout
} // namespace cbag
