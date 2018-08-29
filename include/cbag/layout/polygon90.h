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

class compact_iterator : public std::iterator<std::forward_iterator_tag, coord_t> {
  private:
    point_vector_t::const_iterator start;
    point_vector_t::const_iterator stop;
    bool second;

  public:
    compact_iterator();

    compact_iterator(point_vector_t::const_iterator start, point_vector_t::const_iterator stop,
                     bool second = false);

    coord_t operator*() const { return second ? start->y() : start->x(); }

    compact_iterator &operator++();

    bool operator==(const compact_iterator &rhs) const {
        return start == rhs.start && stop == rhs.stop && second == rhs.second;
    }

    bool operator!=(const compact_iterator &rhs) const {
        return start != rhs.start || stop != rhs.stop || second != rhs.second;
    }
};

class point_iterator : public std::iterator<std::forward_iterator_tag, point_t> {
  private:
    const point_vector_t *ptr;
    std::size_t idx;
    bool between;

  public:
    point_iterator();

    point_iterator(const point_vector_t *ptr, std::size_t idx, bool between);

    point_t operator*() const;

    point_iterator &operator++();

    bool operator==(const point_iterator &rhs) const {
        return ptr == rhs.ptr && idx == rhs.idx && between == rhs.between;
    }

    bool operator!=(const point_iterator &rhs) const {
        return idx != rhs.idx || between != rhs.between || ptr != rhs.ptr;
    }
};

// -----------------------------------------------------------------------------
// polygon90 declaration
// for polygon90, the data vector stores every other vertex, as all vertices
// in between can be inferred.
// -----------------------------------------------------------------------------

class polygon90 : public polygon45 {
  public:
    using compact_iterator_type = compact_iterator;
    using iterator_type = point_iterator;

    polygon90();
    explicit polygon90(std::size_t n);
    explicit polygon90(point_vector_t data, winding_dir wdir = winding_dir::unknown_winding);

    compact_iterator_type begin_compact() const { return {data.begin(), data.end(), false}; }
    compact_iterator_type end_compact() const { return {data.end(), data.end(), true}; }
    iterator_type begin() const { return {&data, 0, false}; }
    iterator_type end() const { return {&data, data.size(), false}; }
    std::size_t size() const { return 2 * data.size(); }

    template <class iT> void set_compact(iT input_begin, iT input_end) {
        data.clear();
        while (input_begin != input_end) {
            coord_t tmp = *input_begin;
            ++input_begin;
            data.emplace_back(tmp, *input_begin);
            ++input_begin;
        }
        wdir = winding_dir::unknown_winding;
    }

    template <class iT> void set(iT input_begin, iT input_end) {
        data.clear();
        while (input_begin != input_end) {
            data.push_back(*input_begin);
            ++input_begin;
            ++input_begin;
        }
        wdir = winding_dir::unknown_winding;
    }
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
