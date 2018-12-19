#include <cbag/common/via_param.h>

namespace cbag {

via_param::via_param() = default;

via_param::via_param(uint32_t vnx, uint32_t vny, dist_t w, dist_t h, offset_t vspx, offset_t vspy,
                     offset_t enc1x, offset_t enc1y, offset_t off1x, offset_t off1y, offset_t enc2x,
                     offset_t enc2y, offset_t off2x, offset_t off2y)
    : num{vnx, vny}, cut_dim{w, h}, cut_spacing(vspx, vspy), lay1_enc(enc1x, enc1y),
      lay1_off(off1x, off1y), lay2_enc(enc2x, enc2y), lay2_off(off2x, off2y) {}

} // namespace cbag
