
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
  private:
    offset_t dx = 0;
    offset_t dy = 0;

  public:
    vector() {}

    vector(offset_t dx, offset_t dy) : dx(dx), dy(dy) {}

    offset_t x() const { return dx; }

    offset_t y() const { return dy; }
};

} // namespace cbag

#endif
#endif
