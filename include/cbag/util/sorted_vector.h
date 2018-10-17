#ifndef CBAG_UTIL_SORTED_VECTOR_H
#define CBAG_UTIL_SORTED_VECTOR_H

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace cbag {
namespace util {

template <class T, class Compare = std::less<T>> class sorted_vector {
  public:
    using value_type = T;
    using size_type = std::size_t;
    using reference = value_type &;
    using iterator = typename std::vector<T>::iterator;
    using const_reference = const value_type &;
    using const_iterator = typename std::vector<T>::const_iterator;

  private:
    std::vector<T> data_;
    Compare comp_;

  public:
    sorted_vector() = default;

    size_type size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    const_iterator begin() const { return data_.begin(); }
    const_iterator end() const { return data_.end(); }
    const_reference at_front() const {
        if (data_.empty())
            throw std::out_of_range("Cannot get front of empty vector.");
        return data_.front();
    }
    const_reference at_back() const {
        if (data_.empty())
            throw std::out_of_range("Cannot get back of empty vector.");
        return data_.back();
    }

    template <class K> const_iterator lower_bound(const K &x) const {
        return std::lower_bound(data_.begin(), data_.end(), x, comp_);
    }

    template <class K> const_iterator upper_bound(const K &x) const {
        return std::upper_bound(data_.begin(), data_.end(), x, comp_);
    }

    template <class K> std::pair<const_iterator, const_iterator> equal_range(const K &x) const {
        return std::equal_range(data_.begin(), data_.end(), x, comp_);
    }

    template <class K> std::pair<iterator, iterator> equal_range(const K &x) {
        return std::equal_range(data_.begin(), data_.end(), x, comp_);
    }

    template <class... Args> std::pair<iterator, bool> emplace_unique(Args &&... args) {
        T item(std::forward<Args>(args)...);

        auto iter_range = equal_range(item);
        if (iter_range.first != iter_range.second)
            return {iter_range.first, false};
        return {data_.insert(iter_range.second, std::move(item)), true};
    }

    std::pair<iterator, bool> insert_unique(T &&item) {
        auto iter_range = equal_range(item);
        if (iter_range.first != iter_range.second)
            return {iter_range.first, false};
        return {data_.insert(iter_range.second, std::move(item)), true};
    }

    iterator erase(const_iterator pos) { return data_.erase(std::move(pos)); }

    iterator erase(const_iterator first, const_iterator last) {
        return data_.erase(std::move(first), std::move(last));
    }
};

} // namespace util
} // namespace cbag

#endif
