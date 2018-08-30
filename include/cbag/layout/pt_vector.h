#ifndef CBAG_LAYOUT_PT_VECTOR_H
#define CBAG_LAYOUT_PT_VECTOR_H

#include <cbag/layout/point_t.h>

namespace cbag {
namespace layout {

/** This file defines a optimized vector of points.
 *
 *  This optmized vector
 *  This point vector is optimized in the sense of total size.  It only takes up 17 bytes (instead
 *  of 24 bytes on 64-bit systems).  The tradeoff is that it can only be 2^32-1 elements long,
 *  and only works on 64-bit systems or below.  Furthermore, it's alignment is only 1 bytes.
 *
 *  The advantage of this point vector class is its compactness.  Furthermore, no memory allocation
 *  will occur on the heap if the number of points is less than 3 (which is a very common case for
 *  rectangles).  Furthermore, the mode flag not only keeps track of the size, but also the winding
 *  direction of the points.
 */
class pt_vector {
  private:
    uint8_t mode = 0;
    point_t points[2];

  public:
    pt_vector();

    pt_vector(uint32_t size);

    pt_vector(coord_t x0, coord_t y0, coord_t x1, coord_t y1);
};

} // namespace layout
} // namespace cbag

#endif
