#ifndef CBAG_UTIL_INTERVAL_H
#define CBAG_UTIL_INTERVAL_H

#include <iterator>
#include <type_traits>
#include <utility>

#include <fmt/core.h>

#include <cbag/common/typedefs.h>
#include <cbag/util/sorted_vector.h>

namespace cbag {
namespace util {

namespace traits {

template <typename T> struct coordinate_type {};
template <typename T> struct interval {};

template <> struct coordinate_type<std::array<offset_t, 2>> { using type = offset_t; };
template <> struct interval<std::array<offset_t, 2>> {
    using intv_type = std::array<offset_t, 2>;
    using coordinate_type = coordinate_type<intv_type>::type;

    static intv_type &intv(intv_type &i) { return i; }
    static const intv_type &intv(const intv_type &i) { return i; }
    static coordinate_type start(const intv_type &i) { return i[0]; }
    static coordinate_type stop(const intv_type &i) { return i[1]; }
    static void set_start(intv_type &i, coordinate_type val) { i[0] = val; }
    static void set_stop(intv_type &i, coordinate_type val) { i[1] = val; }
    static intv_type construct(coordinate_type start, coordinate_type stop) {
        return {start, stop};
    }
};

template <> struct coordinate_type<offset_t> { using type = offset_t; };
template <> struct interval<offset_t> {
    using coordinate_type = coordinate_type<offset_t>::type;

    coordinate_type start(coordinate_type i) { return i; }
    coordinate_type stop(coordinate_type i) { return i + 1; }
};

} // namespace traits

template <typename T> bool nonempty(const T &i) {
    return traits::interval<T>::start(i) < traits::interval<T>::stop(i);
}

template <typename T>
void transform(T &i, typename traits::coordinate_type<T>::type scale,
               typename traits::coordinate_type<T>::type shift) {
    auto start = traits::interval<T>::start(i);
    auto stop = traits::interval<T>::stop(i);
    if (scale >= 0) {
        traits::interval<T>::set_start(i, scale * start + shift);
        traits::interval<T>::set_stop(i, scale * stop + shift);
    } else {
        traits::interval<T>::set_start(i, scale * stop + shift);
        traits::interval<T>::set_stop(i, scale * start + shift);
    }
}

template <typename T1, typename T2> bool operator==(const T1 &lhs, const T2 &rhs) {
    return traits::interval<T1>::start(lhs) == traits::interval<T2>::start(rhs) &&
           traits::interval<T1>::stop(lhs) == traits::interval<T2>::stop(rhs);
}

struct intv_comp {
    using is_transparent = void;

    template <typename T1, typename T2> bool operator()(const T1 &lhs, const T2 &rhs) const {
        return traits::interval<T1>::stop(lhs) <= traits::interval<T2>::start(rhs);
    }
};

template <class Interval = std::array<offset_t, 2>> class disjoint_intvs {
  public:
    using value_type = Interval;
    using coord_type = typename traits::coordinate_type<Interval>::type;
    using vector_type = sorted_vector<value_type, intv_comp>;
    using size_type = typename vector_type::size_type;
    using iterator = typename vector_type::iterator;
    using const_iterator = typename vector_type::const_iterator;

    class const_intv_iterator {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const typename traits::interval<disjoint_intvs::value_type>::intv_type;
        using difference_type = typename const_iterator::difference_type;
        using pointer = value_type *;
        using reference = value_type &;

      private:
        const_iterator iter_;

      public:
        const_intv_iterator() = default;
        const_intv_iterator(const_iterator val) : iter_(std::move(val)) {}

        bool operator==(const const_intv_iterator &other) const { return iter_ == other.iter_; }
        bool operator!=(const const_intv_iterator &other) const { return iter_ != other.iter_; }

        reference operator*() const { return traits::interval<Interval>::intv(*iter_); }
        pointer operator->() const { return &operator*(); }

        const_intv_iterator &operator++() {
            ++iter_;
            return *this;
        }
        const_intv_iterator operator++(int) {
            const_intv_iterator ans(iter_);
            operator++();
            return ans;
        }
    };

  private:
    vector_type data_;

  public:
    disjoint_intvs() = default;

    explicit disjoint_intvs(vector_type &&data) : data_(std::move(data)) {}

  private:
    static void intersect_helper(disjoint_intvs::vector_type &ans, const value_type &intv,
                                 disjoint_intvs::const_iterator &first,
                                 const disjoint_intvs::const_iterator &last,
                                 const intv_comp &comp) {
        auto [start_iter, stop_iter] = std::equal_range(first, last, intv, comp);
        first = (start_iter == stop_iter) ? start_iter : stop_iter - 1;
        for (; start_iter != stop_iter; ++(start_iter)) {
            coord_type start = std::max(traits::interval<Interval>::start(intv),
                                        traits::interval<Interval>::start(*start_iter));
            coord_type stop = std::min(traits::interval<Interval>::stop(intv),
                                       traits::interval<Interval>::stop(*start_iter));
            if (start < stop)
                ans.push_back(traits::interval<Interval>::construct(start, stop));
        }
    }

  public:
    const_iterator begin() const { return data_.begin(); }
    const_iterator end() const { return data_.end(); }

    const_intv_iterator intv_begin() const { return const_intv_iterator(begin()); }
    const_intv_iterator intv_end() const { return const_intv_iterator(end()); }

    template <class K> std::pair<const_iterator, const_iterator> overlap_range(const K &key) const {
        return data_.equal_range(key);
    }
    bool empty() const { return data_.empty(); }
    size_type size() const { return data_.size(); }
    coord_type start() const { return traits::interval<Interval>::start(data_.at_front()); }
    coord_type stop() const { return traits::interval<Interval>::stop(data_.at_back()); }
    template <class K> const_iterator find_exact(const K &key) const {
        return data_.find_exact(key);
    }
    template <class K> bool contains(const K &key) const {
        return data_.find_exact(key) != data_.end();
    }
    template <class K> bool overlaps(const K &key) const { return data_.equal_size(key) > 0; }
    template <class K> bool covers(const K &key) const { return data_.equal_size(key) == 1; }
    disjoint_intvs get_intersection(const disjoint_intvs &other) const {
        auto iter1 = data_.begin();
        auto iter2 = other.data_.begin();
        auto end1 = data_.end();
        auto end2 = other.data_.end();
        const auto &comp = data_.get_compare();

        vector_type ans;
        auto size1 = end1 - iter1;
        auto size2 = end2 - iter2;
        while (size1 > 0 && size2 > 0) {
            if (size1 <= size2) {
                intersect_helper(ans, *iter1, iter2, end2, comp);
                ++iter1;
            } else {
                intersect_helper(ans, *iter2, iter1, end1, comp);
                ++iter2;
            }
            size1 = end1 - iter1;
            size2 = end2 - iter2;
        }
        return disjoint_intvs(std::move(ans));
    }
    template <class K> disjoint_intvs get_complement(const K &key) const {
        coord_type lower = traits::interval<K>::start(key);
        coord_type upper = traits::interval<K>::stop(key);
        vector_type ans;
        if (data_.empty()) {
            ans.push_back(traits::interval<Interval>::construct(lower, upper));
        } else {
            coord_type a = start();
            coord_type b = stop();
            if (a < lower || upper < b) {
                throw std::out_of_range(
                    fmt::format("disjoint_intvs interval [{:d}, {:d}) not covered by [{:d}, {:d})",
                                a, b, lower, upper));
            }
            for (const auto &item : data_) {
                coord_t i_start = traits::interval<Interval>::start(item);
                if (lower < i_start)
                    ans.push_back(traits::interval<Interval>::construct(lower, i_start));
                lower = traits::interval<Interval>::stop(item);
            }
            if (lower < upper)
                ans.push_back(traits::interval<Interval>::construct(lower, upper));
        }
        return disjoint_intvs(std::move(ans));
    }

    disjoint_intvs get_transform(coord_type scale = 1, coord_type shift = 0) const {
        vector_type ans;
        const_iterator first, last;
        if (scale > 0) {
            for (auto first = data_.begin(); first != data_.end(); ++first) {
                value_type item(*first);
                transform(item, scale, shift);
                ans.push_back(std::move(item));
            }
        } else if (scale < 0) {
            for (auto first = data_.rbegin(); first != data_.rend(); ++first) {
                value_type item(*first);
                transform(item, scale, shift);
                ans.push_back(std::move(item));
            }
        } else {
            return {};
        }
        return disjoint_intvs(std::move(ans));
    }

    disjoint_intvs get_copy() const { return disjoint_intvs(*this); }

    template <class K> std::pair<iterator, iterator> overlap_range(const K &key) {
        return data_.equal_range(key);
    }

    void clear() noexcept { data_.clear(); }

    template <class K> bool remove(const K &key) {
        auto iter = data_.find_exact(key);
        if (iter == data_.end())
            return false;
        data_.erase(iter);
        return true;
    }

    template <class K> bool remove_overlaps(const K &key) {
        auto [start_iter, stop_iter] = overlap_range(key);
        if (start_iter == stop_iter)
            return false;
        data_.erase(start_iter, stop_iter);
        return true;
    }

    template <class K> bool subtract(const K &key) {
        auto [start_iter, stop_iter] = overlap_range(key);
        auto overlap_size = stop_iter - start_iter;
        if (overlap_size == 0)
            return false;

        auto k_start = traits::interval<K>::start(key);
        auto k_stop = traits::interval<K>::stop(key);
        coord_type test = traits::interval<Interval>::start(*start_iter);
        if (test < k_start) {
            // perform subtraction on first interval
            traits::interval<Interval>::set_stop(*start_iter, k_start);
            ++start_iter;
            // we're done if there's no more interval
            if ((--overlap_size) == 0)
                return true;
        }

        auto last_iter = stop_iter - 1;
        test = traits::interval<Interval>::stop(*last_iter);
        if (k_stop < test) {
            // perform subtraction on last interval
            traits::interval<Interval>::set_start(*last_iter, k_stop);
            stop_iter = last_iter;
            if ((--overlap_size) == 0)
                return true;
        }

        // erase all completely overlapped intervals
        data_.erase(start_iter, stop_iter);
        return true;
    }

    template <class... Args> bool emplace(bool merge, bool abut, Args &&... args) {
        value_type item(std::forward<Args>(args)...);
        auto i_start = traits::interval<Interval>::start(item);
        auto i_stop = traits::interval<Interval>::stop(item);
        if (!nonempty(item))
            throw std::invalid_argument(
                fmt::format("Cannot add nonempty interval [{:d}, {:d})", i_start, i_stop));
        abut = abut && merge;
        auto [start_iter, stop_iter] =
            (abut) ? overlap_range(std::array<coord_type, 2>{i_start - 1, i_stop + 1})
                   : overlap_range(item);
        if (start_iter == stop_iter) {
            // no overlapping or abutting intervals
            data_.emplace_unique(std::move(item));
            return true;
        } else if (merge) {
            // have overlapping/abutting intervals, and we want to merge
            auto ovl_start = traits::interval<Interval>::start(*start_iter);
            auto ovl_stop = traits::interval<Interval>::stop(*(stop_iter - 1));
            traits::interval<Interval>::set_start(item, std::min(i_start, ovl_start));
            traits::interval<Interval>::set_stop(item, std::max(i_stop, ovl_stop));
            // modify the first overlapping interval
            *start_iter = item;
            // erase the rest
            ++(start_iter);
            if (stop_iter > start_iter)
                data_.erase(start_iter, stop_iter);
            return true;
        }
        // has overlap, and not merging; adding failed.
        return false;
    }
};

} // namespace util
} // namespace cbag

#endif
