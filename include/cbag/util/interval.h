#ifndef CBAG_UTIL_INTERVAL_H
#define CBAG_UTIL_INTERVAL_H

#include <type_traits>
#include <utility>

#include <fmt/format.h>

#include <cbag/common/typedefs.h>
#include <cbag/util/sorted_vector.h>

namespace cbag {
namespace util {

// a subclass of std::pair representing an interval; it knows how to
// compare with a scalar.
using intv_base_type = std::pair<offset_t, offset_t>;

class intv_type : public intv_base_type {
  public:
    using intv_base_type::intv_base_type;

    void transform(offset_t scale, offset_t shift) {
        if (scale >= 0) {
            first = scale * first + shift;
            second = scale * second + shift;
        } else {
            offset_t temp = scale * second + shift;
            second = scale * first + shift;
            first = temp;
        }
    }

    friend bool operator==(const intv_type &lhs, offset_t rhs) {
        return rhs == lhs.first && rhs + 1 == lhs.second;
    }
    friend bool operator==(offset_t lhs, const intv_type &rhs) { return rhs == lhs; }
};

template <class T = intv_type, std::enable_if_t<std::is_base_of_v<intv_type, T>> * = nullptr>
class disjoint_intvs {
  private:
    struct intv_comp;

  public:
    using value_type = T;
    using vector_type = sorted_vector<value_type, intv_comp>;
    using size_type = typename vector_type::size_type;
    using iterator = typename vector_type::iterator;
    using const_iterator = typename vector_type::const_iterator;

  private:
    struct intv_comp {
        using is_transparent = offset_t;
        using value_type = offset_t;

        bool operator()(value_type lhs, const intv_type &rhs) const { return lhs < rhs.first; }
        bool operator()(const intv_type &lhs, value_type rhs) const { return lhs.second <= rhs; }
        bool operator()(const intv_type &lhs, const intv_type &rhs) const {
            return lhs.second <= rhs.first;
        }
    };

    vector_type data_;

  public:
    disjoint_intvs() = default;

    explicit disjoint_intvs(vector_type &&data) : data_(std::move(data)) {}

  private:
    static void intersect_helper(disjoint_intvs::vector_type &ans, const value_type &intv,
                                 disjoint_intvs::const_iterator &first,
                                 const disjoint_intvs::const_iterator &last,
                                 const intv_comp &comp) {
        auto iter_pair = std::equal_range(first, last, intv, comp);
        first = (iter_pair.first == iter_pair.second) ? iter_pair.first : iter_pair.second - 1;
        for (; iter_pair.first != iter_pair.second; ++(iter_pair.first)) {
            coord_t start = std::max(intv.first, iter_pair.first->first.first);
            coord_t stop = std::min(intv.second, iter_pair.first->first.second);
            ans.emplace(std::make_pair(start, stop), nullptr);
        }
    }

  public:
    const_iterator begin() const { return data_.begin(); }
    const_iterator end() const { return data_.end(); }
    template <class K> std::pair<const_iterator, const_iterator> overlap_range(const K &key) const {
        return data_.equal_range(key);
    }
    bool empty() const { return data_.empty(); }
    size_type size() const { return data_.size(); }
    coord_t start() const { return data_.at_front().first; }
    coord_t stop() const { return data_.at_back().second; }
    template <class K> const_iterator find_exact(const K &key) const {
        return data_.find_exact(key);
    }
    template <class K> bool contains(const K &key) const {
        return data_.find_exact(key) != data_.end();
    }
    template <class K> bool overlaps(const K &key) const { return data_.equal_size(key) > 0; }
    template <class K> bool covers(const K &key) const { return data_.equal_size(key) == 1; }
    disjoint_intvs get_intersect(const disjoint_intvs &other) const {
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
                intersect_helper(ans, iter1->first, iter2, end2, comp);
                ++iter1;
            } else {
                intersect_helper(ans, iter2->first, iter1, end1, comp);
                ++iter2;
            }
            size1 = end1 - iter1;
            size2 = end2 - iter2;
        }
        return disjoint_intvs(std::move(ans));
    }
    disjoint_intvs get_complement(coord_t lower, coord_t upper) const {
        vector_type ans;
        if (data_.empty()) {
            ans.emplace_back(lower, upper);
        } else {
            coord_t a = start();
            coord_t b = stop();
            if (a < lower || upper < b) {
                throw std::out_of_range(
                    fmt::format("disjoint_intvs interval [{:d}, {:d}) not covered by [{:d}, {:d})",
                                a, b, lower, upper));
            }
            for (const auto &item : data_) {
                if (lower < item.first)
                    ans.emplace_back(lower, item.first);
                lower = item.second;
            }
            if (lower < upper)
                ans.emplace_back(lower, upper);
        }
        return disjoint_intvs(std::move(ans));
    }

    disjoint_intvs get_transform(coord_t scale, coord_t shift) const {
        vector_type ans;
        const_iterator first, last;
        if (scale > 0) {
            for (auto first = data_.begin(); first != data_.end(); ++first) {
                value_type item(*first);
                item.transform(scale, shift);
                ans.insert_back(std::move(item));
            }
        } else if (scale < 0) {
            for (auto first = data_.rbegin(); first != data_.rend(); ++first) {
                value_type item(*first);
                item.transform(scale, shift);
                ans.insert_back(std::move(item));
            }
        } else {
            return {};
        }
        return disjoint_intvs(std::move(ans));
    }

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
        auto iter_pair = overlap_range(key);
        if (iter_pair.first == iter_pair.second)
            return false;
        data_.erase(iter_pair.first, iter_pair.second);
        return true;
    }

    template <class K> bool subtract(const K &key) {
        auto iter_pair = overlap_range(key);
        auto overlap_size = iter_pair.second - iter_pair.first;
        if (overlap_size == 0)
            return false;

        coord_t test = iter_pair.first->first;
        if (test < key.first) {
            // perform subtraction on first interval
            iter_pair.first->second = key.first;
            ++iter_pair.first;
            // we're done if there's no more interval
            if ((--overlap_size) == 0)
                return true;
        }

        auto last_iter = iter_pair.second - 1;
        test = last_iter->second;
        if (key.second < test) {
            // perform subtraction on last interval
            last_iter->first = key.second;
            iter_pair.second = last_iter;
            if ((--overlap_size) == 0)
                return true;
        }

        // erase all completely overlapped intervals
        data_.erase(iter_pair.first, iter_pair.second);
        return true;
    }

    template <class... Args> bool emplace(bool merge, bool abut, Args &&... args) {
        value_type item(std::forward<Args>(args)...);
        if (item.first >= item.second)
            throw std::invalid_argument(
                fmt::format("Cannot add invalid interval [{:d}, {:d})", item.first, item.second));
        abut = abut && merge;
        auto iter_pair = (abut) ? overlap_range(intv_type(item.first - 1, item.second + 1))
                                : overlap_range(item);
        if (iter_pair.first == iter_pair.second) {
            // no overlapping or abutting intervals
            data_.emplace_unique(std::move(item));
            return true;
        } else if (merge) {
            // have overlapping/abutting intervals, and we want to merge
            item.first = std::min(item.first, iter_pair.first->first);
            item.second = std::max(item.second, (iter_pair.second - 1)->second);
            // modify the first overlapping interval
            *(iter_pair.first) = item;
            // erase the rest
            ++(iter_pair.first);
            if (iter_pair.second > iter_pair.first)
                data_.erase(iter_pair.first, iter_pair.second);
            return true;
        }
        // has overlap, and not merging; adding failed.
        return false;
    }
};

} // namespace util
} // namespace cbag

#endif
