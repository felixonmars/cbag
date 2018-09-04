#ifndef CBAG_LAYOUT_PT_VECTOR_H
#define CBAG_LAYOUT_PT_VECTOR_H

#include <iterator>

#include <cbag/layout/point_t.h>
#include <cbag/layout/typedefs.h>

namespace cbag {
namespace layout {

/** This file defines a optimized vector of points.
 *
 *  This optmized vector
 *  This point vector is optimized in the sense of total size.  It only takes up 17 bytes (instead
 *  of 24 bytes on 64-bit systems).  The tradeoff is that it can only be 2^32-1 elements long,
 *  and only works on 64-bit systems or below.  Another advantage is that it's alignment is only 1
 *  byte.
 *
 *  The advantage of this point vector class is its compactness.  Also, no memory allocation
 *  will occur on the heap if the number of points is less than 3 (which is a very common case for
 *  rectangles).
 *
 *  Note: move constructor/assignment are done using copy-and-swap idiom.  copy constructor will set
 *        capacity = size for the new object.  copy assignment will also not copy capacity;
 *        memory reallocation will only occur if the destination capacity is smaller than source
 *        size.  In this case, the new destination capacity will be set equal to size.
 */
class pt_vector {
  public:
    static constexpr uint32_t INIT_HEAP_SIZE = 8;
    static constexpr uint32_t STACK_SIZE = 2;

    using value_type = point_t;
    using size_type = uint32_t;
    using iterator = point_t *;
    using const_iterator = const point_t *;

  private:
    uint8_t mode = 0;
    point_t points[STACK_SIZE];
    struct helper;

  public:
    ~pt_vector() noexcept;
    pt_vector() noexcept;
    pt_vector(const pt_vector &other);
    pt_vector &operator=(pt_vector other) noexcept;
    pt_vector(pt_vector &&other) noexcept;

    friend void swap(pt_vector &first, pt_vector &second);

    pt_vector(size_type size);

    pt_vector(coord_t x0, coord_t y0, coord_t x1, coord_t y1) noexcept;

    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    size_type size() const;
    const value_type &operator[](size_type idx) const;
    value_type &operator[](size_type idx);

    value_type &emplace_back(coord_t x, coord_t y);
    void reserve(size_type);
    void clear();

    const value_type &at_raw(size_type idx) const { return points[idx]; }
    value_type &at_raw(size_type idx) { return points[idx]; }
};

} // namespace layout
} // namespace cbag

#endif
