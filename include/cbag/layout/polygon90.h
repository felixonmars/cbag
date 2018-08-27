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

class CompactIterator : public std::iterator<std::forward_iterator_tag, coord_t> {
  public:
    inline CompactIterator() = default;

    inline CompactIterator(point_vector_t::const_iterator start,
                           point_vector_t::const_iterator stop, bool second = false)
        : start(std::move(start)), stop(std::move(stop)), second(second) {}

    inline coord_t operator*() const { return second ? start->y() : start->x(); }

    inline CompactIterator &operator++() {
        if (second) {
            ++start;
            second = false;
        } else {
            second = true;
        }
        return *this;
    }

    inline bool operator==(const CompactIterator &rhs) const {
        return start == rhs.start && stop == rhs.stop && second == rhs.second;
    }

    inline bool operator!=(const CompactIterator &rhs) const {
        return start != rhs.start || stop != rhs.stop || second != rhs.second;
    }

  private:
    point_vector_t::const_iterator start;
    point_vector_t::const_iterator stop;
    bool second;
};

class PointIterator : public std::iterator<std::forward_iterator_tag, point_t> {
  public:
    inline PointIterator() : ptr(nullptr), idx(0), between(false) {}

    inline PointIterator(const point_vector_t *ptr, std::size_t idx, bool between)
        : ptr(ptr), idx(idx), between(between) {}

    inline point_t operator*() const {
        if (between) {
            if (idx == ptr->size() - 1) {
                return point_t((*ptr)[0].x(), (*ptr)[idx].y());
            }
            return point_t((*ptr)[idx + 1].x(), (*ptr)[idx].y());
        }
        return (*ptr)[idx];
    }

    inline PointIterator &operator++() {
        if (between) {
            ++idx;
            between = false;
        } else {
            between = true;
        }
        return *this;
    }

    inline bool operator==(const PointIterator &rhs) const {
        return ptr == rhs.ptr && idx == rhs.idx && between == rhs.between;
    }

    inline bool operator!=(const PointIterator &rhs) const {
        return idx != rhs.idx || between != rhs.between || ptr != rhs.ptr;
    }

  private:
    const point_vector_t *ptr;
    std::size_t idx;
    bool between;
};

// -----------------------------------------------------------------------------
// polygon90 declaration
// for polygon90, the data vector stores every other vertex, as all vertices
// in between can be inferred.
// -----------------------------------------------------------------------------

class polygon90 : public polygon45 {
  public:
    using compact_iterator_type = CompactIterator;
    using iterator_type = PointIterator;

    inline polygon90() : polygon45() {}
    explicit inline polygon90(std::size_t n) : polygon45(n) {}
    explicit inline polygon90(point_vector_t data) : polygon45(std::move(data)) {}

    inline compact_iterator_type begin_compact() const { return {data.begin(), data.end(), false}; }
    inline compact_iterator_type end_compact() const { return {data.end(), data.end(), true}; }
    inline iterator_type begin() const { return {&data, 0, false}; }
    inline iterator_type end() const { return {&data, data.size(), false}; }
    inline std::size_t size() const { return 2 * data.size(); }

    template <class iT> inline void set_compact(iT input_begin, iT input_end) {
        data.clear();
        while (input_begin != input_end) {
            coord_t tmp = *input_begin;
            ++input_begin;
            data.emplace_back(tmp, *input_begin);
            ++input_begin;
        }
    }

    template <class iT> inline void set(iT input_begin, iT input_end) {
        data.clear();
        while (input_begin != input_end) {
            data.push_back(*input_begin);
            ++input_begin;
            ++input_begin;
        }
    }
};

using polygon90_set = std::vector<polygon90>;

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::polygon90> { using type = polygon_90_concept; };

} // namespace polygon
} // namespace boost

#endif
