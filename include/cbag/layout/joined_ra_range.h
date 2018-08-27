/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_JOINED_RA_RANGE_H
#define CBAG_LAYOUT_JOINED_RA_RANGE_H

#include <iterator>

namespace cbag {
namespace layout {

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
class joined_ra_range {
  private:
    class joined_ra_iterator
        : public std::iterator<std::random_access_iterator_tag, typename ltype::value_type> {
      public:
        using value_type = typename ltype::value_type;
        using difference_type = typename std::iterator<std::random_access_iterator_tag,
                                                       typename ltype::value_type>::difference_type;

        inline joined_ra_iterator() = default;

        inline joined_ra_iterator(typename ltype::const_iterator lstart,
                                  typename rtype::const_iterator rstart, std::size_t idx,
                                  std::size_t lsize)
            : idx(idx), lsize(lsize), lstart(std::move(lstart)), rstart(std::move(rstart)) {}

        inline joined_ra_iterator *operator+=(difference_type rhs) {
            idx += rhs;
            return *this;
        }
        inline joined_ra_iterator *operator-=(difference_type rhs) {
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
        inline joined_ra_iterator &operator++() {
            ++idx;
            return *this;
        }
        inline joined_ra_iterator &operator--() {
            --idx;
            return *this;
        }
        inline joined_ra_iterator operator++(int) {
            joined_ra_iterator tmp(lstart, rstart, idx, lsize);
            ++idx;
            return tmp;
        }
        inline joined_ra_iterator operator--(int) {
            joined_ra_iterator tmp(lstart, rstart, idx, lsize);
            --idx;
            return tmp;
        }
        inline difference_type operator-(const joined_ra_iterator &rhs) const {
            return idx - rhs.idx;
        }
        inline joined_ra_iterator operator+(difference_type rhs) const {
            return {lstart, rstart, idx + rhs, lsize};
        }
        inline joined_ra_iterator operator-(difference_type rhs) const {
            return {lstart, rstart, idx - rhs, lsize};
        }
        friend inline joined_ra_iterator operator+(difference_type lhs,
                                                   const joined_ra_iterator &rhs) {
            return {rhs.lstart, rhs.rstart, rhs.idx + lhs, rhs.lsize};
        }
        friend inline joined_ra_iterator operator-(difference_type lhs,
                                                   const joined_ra_iterator &rhs) {
            return {rhs.lstart, rhs.rstart, rhs.idx - lhs, rhs.lsize};
        }
        inline bool operator==(const joined_ra_iterator &rhs) const {
            return idx == rhs.idx && lsize == rhs.lsize && lstart == rhs.lstart &&
                   rstart == rhs.rstart;
        }
        inline bool operator!=(const joined_ra_iterator &rhs) const { return !((*this) == rhs); }
        inline bool operator>(const joined_ra_iterator &rhs) const { return idx > rhs.idx; }
        inline bool operator<(const joined_ra_iterator &rhs) const { return idx < rhs.idx; }
        inline bool operator>=(const joined_ra_iterator &rhs) const { return !((*this) < rhs.idx); }
        inline bool operator<=(const joined_ra_iterator &rhs) const { return !((*this) > rhs.idx); }

      private:
        std::size_t idx;
        const std::size_t lsize;
        const typename ltype::const_iterator lstart;
        const typename rtype::const_iterator rstart;
    };

  public:
    using iterator = joined_ra_iterator;
    using const_iterator = iterator;
    using value_type = typename joined_ra_iterator::value_type;

    inline joined_ra_range(const ltype &lval, const rtype &rval) : lval(&lval), rval(&rval) {}

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

} // namespace layout
} // namespace cbag

#endif
