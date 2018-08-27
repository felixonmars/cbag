
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
  public:
    inline transform() : x(0), y(0), oval(oR0) {}

    inline transform(coord_t x, coord_t y, orientation orient = oR0) : x(x), y(y), oval(orient) {}

    inline coord_t xOffset() const { return x; }

    inline coord_t yOffset() const { return y; }

    inline orientation orient() const { return oval; }

    inline void set(coord_t xv, coord_t yv, orientation o) {
        x = xv;
        y = yv;
        oval = o;
    }

  private:
    coord_t x, y;
    orientation oval;
};

} // namespace cbag

#endif
#endif
