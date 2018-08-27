
#ifndef CBAG_COMMON_POINT_H
#define CBAG_COMMON_POINT_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using point = oa::oaPoint;

} // namespace cbag

#else

#include <cbag/common/typedefs.h>

namespace cbag {

class point {
  public:
    inline point() : xv(0), yv(0) {}

    inline point(coord_t xv, coord_t yv) : xv(xv), yv(yv) {}

    inline coord_t x() const { return xv; }

    inline coord_t y() const { return yv; }

    inline void set(coord_t x, coord_t y) {
        xv = x;
        yv = y;
    }

  private:
    coord_t xv, yv;
};

} // namespace cbag

#endif
#endif
