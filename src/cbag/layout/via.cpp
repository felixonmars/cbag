#include <cbag/layout/rectangle.h>
#include <cbag/layout/via.h>

namespace cbag {
namespace layout {

struct via::helper {
    static rectangle get_box(const via &self, const vector &offset, const vector &enc) {
        dist_t via_w = self.num[0] * self.cut_dim[0] + (self.num[0] - 1) * self.cut_spacing.x();
        dist_t via_h = self.num[1] * self.cut_dim[1] + (self.num[1] - 1) * self.cut_spacing.y();

        coord_t xl = offset.x() - (via_w / 2) - enc.x();
        coord_t yl = offset.y() - (via_h / 2) - enc.y();
        coord_t xh = xl + via_w + enc.x();
        coord_t yh = yl + via_h + enc.y();

        rectangle r(xl, yl, xh, yh);
        bp::transform(r, self.xform);
        return r;
    }
};

via::via(transformation xform, const char *via_id, const uint32_t (&num)[2],
         const dist_t (&cut_dim)[2], const offset_t (&cut_sp)[2], const offset_t (&lay1_enc)[2],
         const offset_t (&lay1_off)[2], const offset_t (&lay2_enc)[2],
         const offset_t (&lay2_off)[2])
    : xform(xform), via_id(via_id), num{num[0], num[1]}, cut_dim{cut_dim[0], cut_dim[1]},
      cut_spacing(cut_sp[0], cut_sp[1]), lay1_enc(lay1_enc[0], lay1_enc[1]),
      lay1_off(lay1_off[0], lay1_off[1]), lay2_enc(lay2_enc[0], lay2_enc[1]),
      lay2_off(lay2_off[0], lay2_off[1]) {}

rectangle via::bot_box() const { return helper::get_box(*this, lay1_off, lay1_enc); }

rectangle via::top_box() const { return helper::get_box(*this, lay2_off, lay2_enc); }

void via::set_layer1(polygon_ref<rectangle> ref) { lay1_ref = std::move(ref); }

void via::set_layer2(polygon_ref<rectangle> ref) { lay2_ref = std::move(ref); }

} // namespace layout
} // namespace cbag
