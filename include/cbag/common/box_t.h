
#ifndef CBAG_COMMON_BBOX_H
#define CBAG_COMMON_BBOX_H

#include <cbag/common/typedefs.h>

namespace cbag {

class box_t {
  public:
    coord_t xl = 0;
    coord_t yl = 0;
    coord_t xh = 0;
    coord_t yh = 0;

    box_t();

    box_t(coord_t xl, coord_t yl, coord_t xh, coord_t yh);

    offset_t width() const;

    offset_t height() const;
};

} // namespace cbag

#endif
