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
    using coordinate_type = coord_t;
    using iterator_type = point_vector_t::const_iterator;
    using point_type = point_t;

    inline Polygon() : data() {}
    explicit inline Polygon(std::size_t n) : data() { data.reserve(n); }
    explicit inline Polygon(point_vector_t data) : data(std::move(data)) {}

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
    explicit inline Polygon45(std::size_t n) : Polygon(n) {}
    explicit inline Polygon45(point_vector_t data) : Polygon(std::move(data)) {}
};

// -----------------------------------------------------------------------------
// polygon90 compact/point iterator declaration
// -----------------------------------------------------------------------------

class CompactIterator : public std::iterator<std::forward_iterator_tag, coord_t> {
  public:
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
    using compact_iterator_type = CompactIterator;
    using iterator_type = PointIterator;

    inline Polygon90() : Polygon45() {}
    explicit inline Polygon90(std::size_t n) : Polygon45(n) {}
    explicit inline Polygon90(point_vector_t data) : Polygon45(std::move(data)) {}

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

// -----------------------------------------------------------------------------
// rectangle declaration
// for rectangle, the first point is the lower left vertex, the second point
// is the upper right vertex.
// as the result, rectangle always have exactly 2 points in data.
// -----------------------------------------------------------------------------

class Rect : public Polygon90 {
  public:
    using interval_type = bp::interval_data<coordinate_type>;

    // defaults to 0 area rectangle at origin
    inline Rect() : Polygon90({point_t(0, 0), point_t(0, 0)}) {}
    inline Rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh)
        : Polygon90({point_t(xl, yl), point_t(xh, yh)}) {}

    template <typename T1, typename T2>
    inline Rect(const T1 &hrange, const T2 &vrange) : Polygon90({point_t(0, 0), point_t(0, 0)}) {
        set(bp::HORIZONTAL, hrange);
        set(bp::VERTICAL, vrange);
    }

    inline interval_type get(bp::orientation_2d orient) const {
        if (orient.to_int() == bp::HORIZONTAL) {
            return {data[0].x(), data[1].x()};
        }
        return {data[0].y(), data[1].y()};
    }

    template <typename T2> inline void set(bp::orientation_2d orient, const T2 &interval) {
        data[0].set(orient, bp::low(interval));
        data[1].set(orient, bp::high(interval));
    }
};

// -----------------------------------------------------------------------------
// polygon sets typedefs
// -----------------------------------------------------------------------------

using PolygonSet = std::vector<Polygon>;
using Polygon45Set = std::vector<Polygon45>;
using Polygon90Set = std::vector<Polygon90>;
using RectSet = std::vector<Rect>;

// -----------------------------------------------------------------------------
// JoinedRange declaration
// -----------------------------------------------------------------------------

/** A view of the concatenation of two containers.
 *
 * This class allows you to iterate over the concatenation of two containers.
 * It is different than boost::range::join because this class acts as a view,
 * meaning that it reflects any changes to the underlying containers.
 *
 * The requirements of the uderlying contains are:
 * 1. Define value_type and const_iterator tags.
 * 2. the const_iterator must be random access iterator.
 * 3. the value_type of the second container must derived from the
 *    value_type of the first container.
 * 4. the first container cannot be null (the second container can)
 *
 * The iterator returns value_type of the first container.
 */
template <typename ltype, typename rtype,
          typename std::enable_if_t<
              std::is_same_v<
                  typename std::iterator_traits<typename ltype::const_iterator>::iterator_category,
                  std::random_access_iterator_tag> &&
              std::is_same_v<
                  typename std::iterator_traits<typename rtype::const_iterator>::iterator_category,
                  std::random_access_iterator_tag> &&
              std::is_base_of<typename ltype::value_type, typename rtype::value_type>::value> * =
              nullptr>
class JoinedRARange {
  private:
    class JoinedRAIterator
        : public std::iterator<std::random_access_iterator_tag, typename ltype::value_type> {
      public:
        using value_type = typename ltype::value_type;
        using difference_type = typename std::iterator<std::random_access_iterator_tag,
                                                       typename ltype::value_type>::difference_type;

        inline JoinedRAIterator() = default;

        inline JoinedRAIterator(typename ltype::const_iterator lstart,
                                typename rtype::const_iterator rstart, std::size_t idx,
                                std::size_t lsize)
            : idx(idx), lsize(lsize), lstart(std::move(lstart)), rstart(std::move(rstart)) {}

        inline JoinedRAIterator *operator+=(difference_type rhs) {
            idx += rhs;
            return *this;
        }
        inline JoinedRAIterator *operator-=(difference_type rhs) {
            idx -= rhs;
            return *this;
        }
        inline const typename ltype::value_type &operator*() const {
            return (idx < lsize) ? lstart[idx] : rstart[idx - lsize];
        }
        inline const typename ltype::value_type *operator->() const {
            return (idx < lsize) ? lstart + idx : rstart + (idx - lsize);
        }
        inline const typename ltype::value_type &operator[](difference_type rhs) const {
            std::size_t tmp = idx + rhs;
            return (tmp < lsize) ? lstart[tmp] : rstart[tmp - lsize];
        }
        inline JoinedRAIterator &operator++() {
            ++idx;
            return *this;
        }
        inline JoinedRAIterator &operator--() {
            --idx;
            return *this;
        }
        inline JoinedRAIterator operator++(int) {
            JoinedRAIterator tmp(lstart, rstart, idx, lsize);
            ++idx;
            return tmp;
        }
        inline JoinedRAIterator operator--(int) {
            JoinedRAIterator tmp(lstart, rstart, idx, lsize);
            --idx;
            return tmp;
        }
        inline difference_type operator-(const JoinedRAIterator &rhs) const {
            return idx - rhs.idx;
        }
        inline JoinedRAIterator operator+(difference_type rhs) const {
            return {lstart, rstart, idx + rhs, lsize};
        }
        inline JoinedRAIterator operator-(difference_type rhs) const {
            return {lstart, rstart, idx - rhs, lsize};
        }
        friend inline JoinedRAIterator operator+(difference_type lhs, const JoinedRAIterator &rhs) {
            return {rhs.lstart, rhs.rstart, rhs.idx + lhs, rhs.lsize};
        }
        friend inline JoinedRAIterator operator-(difference_type lhs, const JoinedRAIterator &rhs) {
            return {rhs.lstart, rhs.rstart, rhs.idx - lhs, rhs.lsize};
        }
        inline bool operator==(const JoinedRAIterator &rhs) const {
            return idx == rhs.idx && lsize == rhs.lsize && lstart == rhs.lstart &&
                   rstart == rhs.rstart;
        }
        inline bool operator!=(const JoinedRAIterator &rhs) const { return !((*this) == rhs); }
        inline bool operator>(const JoinedRAIterator &rhs) const { return idx > rhs.idx; }
        inline bool operator<(const JoinedRAIterator &rhs) const { return idx < rhs.idx; }
        inline bool operator>=(const JoinedRAIterator &rhs) const { return !((*this) < rhs.idx); }
        inline bool operator<=(const JoinedRAIterator &rhs) const { return !((*this) > rhs.idx); }

      private:
        std::size_t idx;
        const std::size_t lsize;
        const typename ltype::const_iterator lstart;
        const typename rtype::const_iterator rstart;
    };

  public:
    using iterator = JoinedRAIterator;
    using const_iterator = iterator;
    using value_type = typename JoinedRAIterator::value_type;

    inline JoinedRARange(const ltype &lval, const rtype &rval) : lval(&lval), rval(&rval) {}

    inline const_iterator begin() const {
        return const_iterator(lval->begin(), rval->begin(), 0, lval->size());
    }
    inline const_iterator end() const {
        std::size_t tmp = lval->size();
        return const_iterator(lval->begin(), rval->begin(), tmp + rval->size(), tmp);
    }
    inline std::size_t size() const { return lval->size() + rval->size(); }

  private:
    const ltype *lval;
    const rtype *rval;
};

// -----------------------------------------------------------------------------
// various set views/union views declarations
// -----------------------------------------------------------------------------

class RectView {
  public:
    using value_type = Rect;
    using const_iterator = RectSet::const_iterator;
    using coordinate_type = coord_t;
    using operator_arg_type = RectView;

    inline RectView() = default;

    inline explicit RectView(const RectSet &val) : ptr(&val) {}

    inline const_iterator begin() const { return ptr->begin(); }

    inline const_iterator end() const { return ptr->end(); }

    inline std::size_t size() const { return ptr->size(); }

  private:
    const RectSet *ptr;
};

class Poly90View : public JoinedRARange<Polygon90Set, RectSet> {
  public:
    using coordinate_type = coord_t;
    using operator_arg_type = Poly90View;

    inline Poly90View(const Polygon90Set &lval, const RectSet &rval)
        : JoinedRARange<Polygon90Set, RectSet>(lval, rval) {}
};

class Poly45View : public JoinedRARange<Polygon45Set, Poly90View> {
  public:
    using coordinate_type = coord_t;
    using operator_arg_type = Poly45View;

    inline Poly45View(const Polygon45Set &val0, const Polygon90Set &val1, const RectSet &val2)
        : JoinedRARange<Polygon45Set, Poly90View>(val0, view_90), view_90(val1, val2) {}

  private:
    Poly90View view_90;
};

class PolyView : public JoinedRARange<JoinedRARange<PolygonSet, Polygon45Set>, Poly90View> {
  public:
    using coordinate_type = coord_t;
    using operator_arg_type = PolyView;

    inline PolyView(const PolygonSet &val0, const Polygon45Set &val1, const Polygon90Set &val2,
                    const RectSet &val3)
        : JoinedRARange<JoinedRARange<PolygonSet, Polygon45Set>, Poly90View>(view_no_90, view_90),
          view_no_90(val0, val1), view_90(val2, val3) {}

  private:
    JoinedRARange<PolygonSet, Polygon45Set> view_no_90;
    Poly90View view_90;
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

// -----------------------------------------------------------------------------
// geometry concept declarations
// -----------------------------------------------------------------------------

template <> struct geometry_concept<cbag::layout::Polygon> { using type = polygon_concept; };

template <> struct geometry_concept<cbag::layout::Polygon45> { using type = polygon_45_concept; };

template <> struct geometry_concept<cbag::layout::Polygon90> { using type = polygon_90_concept; };

template <> struct geometry_concept<cbag::layout::Rect> { using type = rectangle_concept; };

template <> struct geometry_concept<cbag::layout::PolyView> { using type = polygon_set_concept; };

template <> struct geometry_concept<cbag::layout::Poly45View> {
    using type = polygon_45_set_concept;
};

template <> struct geometry_concept<cbag::layout::Poly90View> {
    using type = polygon_90_set_concept;
};

template <> struct geometry_concept<cbag::layout::RectView> {
    using type = polygon_90_set_concept;
};

// -----------------------------------------------------------------------------
// traits declarations
// we do not have to define traits for polygon classes, because they conform
// with the default behavior.
// -----------------------------------------------------------------------------

template <> struct polygon_set_traits<cbag::layout::PolyView> {
    using coordinate_type = typename cbag::layout::PolyView::coordinate_type;
    using iterator_type = typename cbag::layout::PolyView::const_iterator;
    using operator_arg_type = typename cbag::layout::PolyView::operator_arg_type;

    static inline iterator_type begin(const operator_arg_type &set) { return set.begin(); }
    static inline iterator_type end(const operator_arg_type &set) { return set.end(); }

    static inline bool clean(const operator_arg_type &set) { return false; }
    static inline bool sorted(const operator_arg_type &set) { return false; }
};

template <> struct polygon_45_set_traits<cbag::layout::Poly45View> {
    using coordinate_type = typename cbag::layout::Poly45View::coordinate_type;
    using iterator_type = typename cbag::layout::Poly45View::const_iterator;
    using operator_arg_type = typename cbag::layout::Poly45View::operator_arg_type;

    static inline iterator_type begin(const operator_arg_type &set) { return set.begin(); }
    static inline iterator_type end(const operator_arg_type &set) { return set.end(); }

    static inline bool clean(const operator_arg_type &set) { return false; }
    static inline bool sorted(const operator_arg_type &set) { return false; }
};

template <> struct polygon_90_set_traits<cbag::layout::Poly90View> {
    using coordinate_type = typename cbag::layout::Poly90View::coordinate_type;
    using iterator_type = typename cbag::layout::Poly90View::const_iterator;
    using operator_arg_type = typename cbag::layout::Poly90View::operator_arg_type;

    static inline iterator_type begin(const operator_arg_type &set) { return set.begin(); }
    static inline iterator_type end(const operator_arg_type &set) { return set.end(); }

    static inline bool clean(const operator_arg_type &set) { return false; }
    static inline bool sorted(const operator_arg_type &set) { return false; }
};

template <> struct polygon_90_set_traits<cbag::layout::RectView> {
    using coordinate_type = typename cbag::layout::RectView::coordinate_type;
    using iterator_type = typename cbag::layout::RectView::const_iterator;
    using operator_arg_type = typename cbag::layout::RectView::operator_arg_type;

    static inline iterator_type begin(const operator_arg_type &set) { return set.begin(); }
    static inline iterator_type end(const operator_arg_type &set) { return set.end(); }

    static inline bool clean(const operator_arg_type &set) { return false; }
    static inline bool sorted(const operator_arg_type &set) { return false; }
};

} // namespace polygon
} // namespace boost

#endif // CBAG_DATABASE_LAYOUT_DATATYPES_H
