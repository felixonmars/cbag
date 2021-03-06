#include <cbag/common/box_t_util.h>
#include <cbag/common/vector.h>
#include <cbag/layout/via.h>

namespace cbag {
namespace layout {

struct via::helper {
    static box_t get_box(const via &self, const vector &offset, const vector &enc) {
        auto nx = self.params.num[0];
        auto ny = self.params.num[1];
        auto via_w = nx * self.params.cut_dim[0] + (nx - 1) * self.params.cut_spacing[0];
        auto via_h = ny * self.params.cut_dim[1] + (ny - 1) * self.params.cut_spacing[1];

        auto xl = static_cast<coord_t>(offset[0] - (via_w / 2) - enc[0]);
        auto yl = static_cast<coord_t>(offset[1] - (via_h / 2) - enc[1]);
        auto xh = static_cast<coord_t>(xl + via_w + enc[0]);
        auto yh = static_cast<coord_t>(yl + via_h + enc[1]);

        box_t r{xl, yl, xh, yh};
        transform(r, self.xform);
        return r;
    }
};

via::via() = default;

via::via(cbag::transformation xform, std::string via_id, via_param params)
    : via_id(std::move(via_id)), params(std::move(params)), xform(std::move(xform)) {}

const std::string &via::get_via_id() const { return via_id; }

const via_param &via::get_params() const { return params; }

bool via::operator==(const via &rhs) const noexcept {
    return via_id == rhs.via_id && params == rhs.params && xform == rhs.xform;
}

} // namespace layout
} // namespace cbag
