/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_POLYGON90_H
#define CBAG_LAYOUT_POLYGON90_H

#include <cbag/layout/polygon45.h>

namespace cbag {
namespace layout {

namespace poly90_iter {

class compact_iterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = coord_t;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type *;
    using reference = const value_type &;

  private:
    pt_vector::const_iterator iter;
    uint8_t parity = 0;

  public:
    compact_iterator();

    compact_iterator(pt_vector::const_iterator iter, uint8_t parity);

    compact_iterator &operator+=(difference_type rhs);
    compact_iterator &operator-=(difference_type rhs);
    const value_type operator*() const;
    pointer operator->() const;
    const value_type operator[](difference_type rhs) const;
    compact_iterator &operator++();
    compact_iterator &operator--();
    compact_iterator operator++(int);
    compact_iterator operator--(int);
    difference_type operator-(const compact_iterator &rhs) const;
    compact_iterator operator+(difference_type rhs) const;
    compact_iterator operator-(difference_type rhs) const;
    friend compact_iterator operator+(difference_type lhs, const compact_iterator &rhs);
    bool operator==(const compact_iterator &rhs) const;
    bool operator!=(const compact_iterator &rhs) const;
    bool operator>(const compact_iterator &rhs) const;
    bool operator<(const compact_iterator &rhs) const;
    bool operator>=(const compact_iterator &rhs) const;
    bool operator<=(const compact_iterator &rhs) const;
};

class point_iterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = point_t;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type *;
    using reference = const value_type &;

  private:
    pt_vector::const_iterator start;
    pt_vector::size_type size = 0;
    uint64_t idx = 0;
    value_type pt;

  public:
    point_iterator();

    point_iterator(pt_vector::const_iterator start, pt_vector::size_type size, uint64_t idx);

    point_iterator &operator+=(difference_type rhs);
    point_iterator &operator-=(difference_type rhs);
    reference operator*() const;
    pointer operator->() const;
    const value_type operator[](difference_type rhs) const;
    point_iterator &operator++();
    point_iterator &operator--();
    point_iterator operator++(int);
    point_iterator operator--(int);
    difference_type operator-(const point_iterator &rhs) const;
    point_iterator operator+(difference_type rhs) const;
    point_iterator operator-(difference_type rhs) const;
    friend point_iterator operator+(difference_type lhs, const point_iterator &rhs);
    bool operator==(const point_iterator &rhs) const;
    bool operator!=(const point_iterator &rhs) const;
    bool operator>(const point_iterator &rhs) const;
    bool operator<(const point_iterator &rhs) const;
    bool operator>=(const point_iterator &rhs) const;
    bool operator<=(const point_iterator &rhs) const;

  private:
    void set_point();
};

} // namespace poly90_iter

// -----------------------------------------------------------------------------
// polygon90 declaration
// for polygon90, the data vector stores every other vertex, as all vertices
// in between can be inferred.
// -----------------------------------------------------------------------------

class polygon90 : public polygon45 {
  public:
    using compact_iterator_type = poly90_iter::compact_iterator;
    using iterator_type = poly90_iter::point_iterator;

    polygon90();
    explicit polygon90(std::size_t n);
    explicit polygon90(pt_vector data, winding_dir wdir = winding_dir::unknown_winding);

    compact_iterator_type begin_compact() const;
    compact_iterator_type end_compact() const;
    iterator_type begin() const;
    iterator_type end() const;
    std::size_t size() const { return 2 * data.size(); }
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::polygon90> { using type = polygon_90_concept; };

// specialize traits to override winding
template <> struct polygon_90_traits<cbag::layout::polygon90> {
    using coordinate_type = cbag::layout::polygon90::coordinate_type;
    using compact_iterator_type = cbag::layout::polygon90::compact_iterator_type;

    static inline compact_iterator_type begin_compact(const cbag::layout::polygon90 &t) {
        return t.begin_compact();
    }

    static inline compact_iterator_type end_compact(const cbag::layout::polygon90 &t) {
        return t.end_compact();
    }

    static inline std::size_t size(const cbag::layout::polygon90 &t) { return t.size(); }

    static inline winding_direction winding(const cbag::layout::polygon90 &t) {
        return t.winding();
    }
};

// specialize traits to override point iterator and winding
template <> struct polygon_traits<cbag::layout::polygon90> {
    using coordinate_type = cbag::layout::polygon90::coordinate_type;
    using iterator_type = cbag::layout::polygon90::iterator_type;
    using point_type = cbag::layout::polygon90::point_type;

    static inline iterator_type begin_points(const cbag::layout::polygon90 &t) { return t.begin(); }

    static inline iterator_type end_points(const cbag::layout::polygon90 &t) { return t.end(); }

    static inline std::size_t size(const cbag::layout::polygon90 &t) { return t.size(); }

    static inline winding_direction winding(const cbag::layout::polygon90 &t) {
        return t.winding();
    }
};

} // namespace polygon
} // namespace boost

#endif
