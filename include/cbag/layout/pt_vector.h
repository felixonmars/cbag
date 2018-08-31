#ifndef CBAG_LAYOUT_PT_VECTOR_H
#define CBAG_LAYOUT_PT_VECTOR_H

#include <iterator>

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
    class pt_iterator {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = point_t;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type *;
        using reference = const value_type &;

      private:
        point_t *ptr = nullptr;

      public:
        pt_iterator();

        pt_iterator(point_t *ptr);

        pt_iterator &operator+=(difference_type rhs);
        pt_iterator &operator-=(difference_type rhs);
        reference operator*() const;
        pointer operator->() const;
        reference operator[](difference_type rhs) const;
        pt_iterator &operator++();
        pt_iterator &operator--();
        pt_iterator operator++(int);
        pt_iterator operator--(int);
        difference_type operator-(const pt_iterator &rhs) const;
        pt_iterator operator+(difference_type rhs) const;
        pt_iterator operator-(difference_type rhs) const;
        friend pt_iterator operator+(difference_type lhs, const pt_iterator &rhs);
        bool operator==(const pt_iterator &rhs) const;
        bool operator!=(const pt_iterator &rhs) const;
        bool operator>(const pt_iterator &rhs) const;
        bool operator<(const pt_iterator &rhs) const;
        bool operator>=(const pt_iterator &rhs) const;
        bool operator<=(const pt_iterator &rhs) const;
    };

  public:
    using value_type = point_t;
    using size_type = uint32_t;
    using const_iterator = pt_iterator;

  private:
    uint8_t mode = 0;
    point_t points[2];

  public:
    ~pt_vector();
    pt_vector();
    pt_vector(const pt_vector &other);
    pt_vector &operator=(const pt_vector &other);
    pt_vector(pt_vector &&other);
    pt_vector &operator=(pt_vector &&other);

    pt_vector(size_type size);

    pt_vector(coord_t x0, coord_t y0, coord_t x1, coord_t y1);

    const_iterator begin() const;
    const_iterator end() const;
    size_t size() const;

    const value_type &operator[](size_t idx) const;
    value_type &operator[](size_t idx);

    value_type &emplace_back(coord_t x, coord_t y);

    void clear();
};

} // namespace layout
} // namespace cbag

#endif
