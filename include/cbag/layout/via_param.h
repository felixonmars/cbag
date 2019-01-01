
#ifndef CBAG_COMMON_VIA_PARAM_H
#define CBAG_COMMON_VIA_PARAM_H

#include <array>

#include <cbag/common/typedefs.h>
#include <cbag/common/vector.h>

namespace cbag {
namespace layout {

struct via_param {
  public:
    std::array<cnt_t, 2> num = {0, 0};
    vector cut_dim = {0, 0};
    vector cut_spacing = {0, 0};
    std::array<vector, 2> enc = {vector{0, 0}, vector{0, 0}};
    std::array<vector, 2> off = {vector{0, 0}, vector{0, 0}};

  public:
    via_param();

    via_param(cnt_t vnx, cnt_t vny, offset_t w, offset_t h, offset_t vspx, offset_t vspy,
              offset_t enc1l, offset_t enc1r, offset_t enc1t, offset_t enc1b, offset_t enc2l,
              offset_t enc2r, offset_t enc2t, offset_t enc2b);
};

} // namespace layout
} // namespace cbag

#endif
