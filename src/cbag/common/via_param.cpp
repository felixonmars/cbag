#include <cbag/common/via_param.h>

namespace cbag {

void set_via_enc_offset(vector &enc, vector &off, offset_t encl, offset_t encr, offset_t enct,
                        offset_t encb) {
    enc.first = (encr + encl) / 2;
    enc.second = (enct + encb) / 2;
    off.first = encr - enc.first;
    off.second = enct - enc.second;
}

via_param::via_param() = default;

via_param::via_param(uint32_t vnx, uint32_t vny, dist_t w, dist_t h, offset_t vspx, offset_t vspy,
                     offset_t enc1l, offset_t enc1r, offset_t enc1t, offset_t enc1b, offset_t enc2l,
                     offset_t enc2r, offset_t enc2t, offset_t enc2b)
    : num{vnx, vny}, cut_dim{w, h}, cut_spacing(vspx, vspy) {
    set_via_enc_offset(lay1_enc, lay1_off, enc1l, enc1r, enc1t, enc1b);
    set_via_enc_offset(lay2_enc, lay2_off, enc2l, enc2r, enc2t, enc2b);
}

} // namespace cbag
