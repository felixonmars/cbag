
#ifndef CBAG_COMMON_BOX_T_H
#define CBAG_COMMON_BOX_T_H

#include <array>

#include <cbag/common/orient_2d.h>
#include <cbag/common/typedefs.h>

namespace cbag {

struct box_t {
  public:
    std::array<coord_t, 2> intvs[2];

    box_t() noexcept;
    box_t(coord_t xl, coord_t yl, coord_t xh, coord_t yh) noexcept;
    box_t(orient_2d orient, coord_t tl, coord_t th, coord_t pl, coord_t ph) noexcept;

    static box_t get_invalid_box() noexcept;
};

} // namespace cbag

#endif
