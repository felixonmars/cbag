
#ifndef CBAG_COMMON_VECTOR_H
#define CBAG_COMMON_VECTOR_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using vector = oa::oaVector;

} // namespace cbag

#else

#include <cbag/common/typedefs.h>

namespace cbag {

class vector {
  public:
    inline vector() : dx(0), dy(0) {}

    inline vector(offset_t dx, offset_t dy) : dx(dx), dy(dy) {}

    inline offset_t x() const { return dx; }

    inline offset_t y() const { return dy; }

  private:
    offset_t dx, dy;
};

} // namespace cbag

#endif
#endif
