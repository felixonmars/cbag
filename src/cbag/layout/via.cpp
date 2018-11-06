#include <cbag/common/vector.h>
#include <cbag/layout/rectangle.h>
#include <cbag/layout/via.h>

namespace cbag {
namespace layout {

struct via::helper {
    static rectangle get_box(const via &self, const vector &offset, const vector &enc) {
        uint32_t nx = self.params.num[0];
        uint32_t ny = self.params.num[1];
        dist_t via_w = nx * self.params.cut_dim[0] + (nx - 1) * self.params.cut_spacing.first;
        dist_t via_h = ny * self.params.cut_dim[1] + (ny - 1) * self.params.cut_spacing.second;

        coord_t xl = offset.first - (via_w / 2) - enc.first;
        coord_t yl = offset.second - (via_h / 2) - enc.second;
        coord_t xh = xl + via_w + enc.first;
        coord_t yh = yl + via_h + enc.second;

        rectangle r(xl, yl, xh, yh);
        bp::transform(r, self.xform);
        return r;
    }
};

via::via() = default;

via::via(cbag::transformation xform, const char *via_id, const uint32_t (&num)[2],
         const dist_t (&cut_dim)[2], const offset_t (&cut_sp)[2], const offset_t (&lay1_enc)[2],
         const offset_t (&lay1_off)[2], const offset_t (&lay2_enc)[2],
         const offset_t (&lay2_off)[2], bool add_layers, bool bot_horiz, bool top_horiz)
    : xform(std::move(xform)), via_id(via_id), add_layers(add_layers), bot_horiz(bot_horiz),
      top_horiz(top_horiz) {
    params.num[0] = num[0];
    params.num[1] = num[1];
    params.cut_dim[0] = cut_dim[0];
    params.cut_dim[1] = cut_dim[1];
    params.cut_spacing = {cut_sp[0], cut_sp[1]};
    params.lay1_enc = {lay1_enc[0], lay1_enc[1]};
    params.lay2_enc = {lay2_enc[0], lay2_enc[1]};
    params.lay1_off = {lay1_off[0], lay1_off[1]};
    params.lay2_off = {lay2_off[0], lay2_off[1]};
}

rectangle via::bot_box() const { return helper::get_box(*this, params.lay1_off, params.lay1_enc); }

rectangle via::top_box() const { return helper::get_box(*this, params.lay2_off, params.lay2_enc); }

} // namespace layout
} // namespace cbag
