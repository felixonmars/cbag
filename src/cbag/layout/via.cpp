#include <cbag/common/vector.h>
#include <cbag/layout/rectangle.h>
#include <cbag/layout/via.h>

namespace cbag {
namespace layout {

struct via::helper {
    static rectangle get_box(const via &self, const vector &offset, const vector &enc) {
        uint32_t nx = self.params.getCutColumns();
        uint32_t ny = self.params.getCutRows();
        auto cut_spacing = self.params.getCutSpacing();
        dist_t via_w = nx * self.params.getCutWidth() + (nx - 1) * cut_spacing.x();
        dist_t via_h = ny * self.params.getCutHeight() + (ny - 1) * cut_spacing.y();

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
    : xform(xform), via_id(via_id) {
    params.setCutColumns(num[0]);
    params.setCutRows(num[1]);
    params.setCutWidth(cut_dim[0]);
    params.setCutHeight(cut_dim[1]);
    params.setCutSpacing(cbag::vector(cut_sp[0], cut_sp[1]));
    params.setLayer1Enc(cbag::vector(lay1_enc[0], lay1_enc[1]));
    params.setLayer1Offset(cbag::vector(lay1_off[0], lay1_off[1]));
    params.setLayer2Enc(cbag::vector(lay2_enc[0], lay2_enc[1]));
    params.setLayer2Offset(cbag::vector(lay2_off[0], lay2_off[1]));
}

rectangle via::bot_box() const {
    return helper::get_box(*this, params.getLayer1Offset(), params.getLayer1Enc());
}

rectangle via::top_box() const {
    return helper::get_box(*this, params.getLayer2Offset(), params.getLayer2Enc());
}

void via::set_layer1(vector_obj_ref<rectangle> ref) { lay1_ref = std::move(ref); }

void via::set_layer2(vector_obj_ref<rectangle> ref) { lay2_ref = std::move(ref); }

} // namespace layout
} // namespace cbag
