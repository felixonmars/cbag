
#ifndef CBAG_COMMON_BBOX_H
#define CBAG_COMMON_BBOX_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using box_t = oa::oaBox;

} // namespace cbag

#else

#include <cbag/common/typedefs.h>

namespace cbag {

class box_t {
  private:
    coord_t xl = 0;
    coord_t yl = 0;
    coord_t xh = 0;
    coord_t yh = 0;

  public:
    box_t() {}

    box_t(coord_t xl, coord_t yl, coord_t xh, coord_t yh) : xl(xl), yl(yl), xh(xh), yh(yh) {}

    coord_t left() const { return xl; }

    coord_t bottom() const { return yl; }

    coord_t right() const { return xh; }

    coord_t top() const { return yh; }

    void set(coord_t x0, coord_t y0, coord_t x1, coord_t y1) {
        xl = x0;
        yl = y0;
        xh = x1;
        yh = y1;
    }
};

} // namespace cbag

#endif
#endif
