
#ifndef CBAG_COMMON_BOX_T_H
#define CBAG_COMMON_BOX_T_H

#include <array>

#include <cbag/common/typedefs.h>
#include <cbag/enum/orient_2d.h>

namespace cbag {

using interval_t = std::array<coord_t, 2>;

struct box_t {
  public:
    std::array<interval_t, 2> intvs;

    box_t() noexcept;
    box_t(coord_t xl, coord_t yl, coord_t xh, coord_t yh) noexcept;
    box_t(orient_2d orient, coord_t tl, coord_t th, coord_t pl, coord_t ph) noexcept;

    static box_t get_invalid_box() noexcept;

    bool operator==(const box_t &rhs) const noexcept;
};

std::ostream &operator<<(std::ostream &os, const box_t &value);

} // namespace cbag

#endif
