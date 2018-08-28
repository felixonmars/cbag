
#ifndef CBAG_COMMON_TRANSFORM_H
#define CBAG_COMMON_TRANSFORM_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using transform = oa::oaTransform;

} // namespace cbag

#else

#include <cbag/common/orientation.h>
#include <cbag/common/typedefs.h>

namespace cbag {

class transform {
  private:
    coord_t x, y;
    orientation oval;

  public:
    transform() : x(0), y(0), oval(oR0) {}

    transform(coord_t x, coord_t y, orientation orient = oR0) : x(x), y(y), oval(orient) {}

    coord_t xOffset() const { return x; }

    coord_t yOffset() const { return y; }

    orientation orient() const { return oval; }

    void set(coord_t xv, coord_t yv, orientation o) {
        x = xv;
        y = yv;
        oval = o;
    }
};

} // namespace cbag

#endif
#endif
