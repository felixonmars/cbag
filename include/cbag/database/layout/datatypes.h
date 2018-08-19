/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_DATABASE_LAYOUT_DATATYPES_H
#define CBAG_DATABASE_LAYOUT_DATATYPES_H

#include <iterator>
#include <vector>

#include <boost/polygon/polygon.hpp>

#include <cbag/database/primitives.h>

namespace cbag {
namespace layout {

// -----------------------------------------------------------------------------
// namespace/typedef declarations
// -----------------------------------------------------------------------------

namespace bp = boost::polygon;

using coord_t = cbag::coord_t;
using point_t = bp::point_data<coord_t>;
using point_vector_t = std::vector<point_t>;

// -----------------------------------------------------------------------------
// polygon/polygon45 declarations
// -----------------------------------------------------------------------------

class Polygon {
  public:
    typedef coord_t coordinate_type;
    typedef point_vector_t::const_iterator iterator_type;
    typedef point_t point_type;

    inline Polygon() : data() {}

    inline iterator_type begin() const { return data.begin(); }
    inline iterator_type end() const { return data.end(); }
    inline std::size_t size() const { return data.size(); }

    template <class iT> inline void set(iT input_begin, iT input_end) {
        data.clear();
        data.insert(data.end(), input_begin, input_end);
    }

  protected:
    point_vector_t data;
};

class Polygon45 : public Polygon {
  public:
    inline Polygon45() : Polygon() {}
};

// -----------------------------------------------------------------------------
// polygon90 compact/point iterator declaration
// -----------------------------------------------------------------------------

class CompactIterator
    : public std::iterator<std::forward_iterator_tag, coord_t> {
  public:
    inline CompactIterator(point_vector_t::const_iterator start,
                           point_vector_t::const_iterator stop,
                           bool second = false)
        : start(std::move(start)), stop(std::move(stop)), second(second) {}

    inline coord_t operator*() const {
        return second ? start->y() : start->x();
    }

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
    const point_vector_t::const_iterator stop;
    bool second;
};

class PointIterator : public std::iterator<std::forward_iterator_tag, point_t> {
  public:
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

class Polygon90 : public Polygon45 {
  public:
    typedef CompactIterator compact_iterator_type;
    typedef PointIterator iterator_type;

    inline Polygon90() : Polygon45() {}

    inline compact_iterator_type begin_compact() const {
        return {data.begin(), data.end(), false};
    }
    inline compact_iterator_type end_compact() const {
        return {data.end(), data.end(), true};
    }
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

// -----------------------------------------------------------------------------
// rectangle declaration
// for rectangle, the first point is the lower left vertex, the second point
// is the upper right vertex.
// as the result, rectangle always have exactly 2 points in data.
// -----------------------------------------------------------------------------

class Rect : public Polygon90 {
  public:
    typedef bp::interval_data<coordinate_type> interval_type;

    inline Rect() : Polygon90() {
        // default to 0 area rectangle at origin
        data.reserve(2);
        data.emplace_back(0, 0);
        data.emplace_back(0, 0);
    }

    template <typename T1, typename T2>
    inline Rect(const T1 &hrange, const T2 &vrange) {
        data.resize(2);
        set(bp::HORIZONTAL, hrange);
        set(bp::VERTICAL, vrange);
    }

    inline interval_type get(bp::orientation_2d orient) const {
        if (orient.to_int() == bp::HORIZONTAL) {
            return {data[0].x(), data[1].x()};
        }
        return {data[0].y(), data[1].y()};
    }

    template <typename T2>
    inline void set(bp::orientation_2d orient, const T2 &interval) {
        data[0].set(orient, bp::low(interval));
        data[1].set(orient, bp::high(interval));
    }
};

} // namespace layout
} // namespace cbag

// boost polygon trait definitions
namespace boost {
namespace polygon {
template <> struct geometry_concept<cbag::layout::Polygon> {
    typedef polygon_concept type;
};

template <> struct geometry_concept<cbag::layout::Polygon45> {
    typedef polygon_45_concept type;
};

template <> struct geometry_concept<cbag::layout::Polygon90> {
    typedef polygon_90_concept type;
};

template <> struct geometry_concept<cbag::layout::Rect> {
    typedef rectangle_concept type;
};

} // namespace polygon
} // namespace boost

#endif // CBAG_DATABASE_LAYOUT_DATATYPES_H
