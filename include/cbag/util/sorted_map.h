#ifndef CBAG_UTIL_SORTED_MAP_H
#define CBAG_UTIL_SORTED_MAP_H

#include <utility>

#include <cbag/util/sorted_vector.h>

namespace cbag {
namespace util {

template <class Key, class T, class Compare = std::less<Key>> class sorted_map {
  public:
    struct value_compare;
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<key_type, mapped_type>;
    using vector_type = sorted_vector<value_type, value_compare>;
    using size_type = typename vector_type::size_type;
    using difference_type = typename vector_type::difference_type;
    using key_compare = Compare;
    using reference = value_type &;
    using const_reference = const value_type &;
    using iterator = typename vector_type::iterator;
    using const_iterator = typename vector_type::const_iterator;
    using reverse_iterator = typename vector_type::reverse_iterator;
    using const_reverse_iterator = typename vector_type::const_reverse_iterator;

    struct value_compare {
        using is_transparent = void;

        key_compare key_comp_;

        template <class K> bool operator()(const value_type &lhs, const K &rhs) const {
            return key_comp_(lhs.first, rhs);
        }
        template <class K> bool operator()(const K &lhs, const value_type &rhs) const {
            return key_comp_(lhs, rhs.first);
        }
        bool operator()(const value_type &lhs, const value_type &rhs) const {
            return key_comp_(lhs.first, rhs.first);
        }
        template <class K> bool operator()(const K &lhs, const K &rhs) const {
            return key_comp_(lhs, rhs);
        }
    };

  private:
    vector_type data_;

  public:
    sorted_map() = default;

    size_type size() const noexcept { return data_.size(); }
    size_type capacity() const noexcept { return data_.capacity(); }
    bool empty() const noexcept { return data_.empty(); }
    const_iterator begin() const noexcept { return data_.begin(); }
    const_iterator end() const noexcept { return data_.end(); }
    iterator begin() noexcept { return data_.begin(); }
    iterator end() noexcept { return data_.end(); }

    mapped_type operator[](const key_type &key) {
        auto iter_range = data_.equal_range(key);
        if (iter_range.first == iter_range.second) {
            auto iter = data_.insert_force(iter_range.first, value_type(key, mapped_type()));
            return iter->second;
        } else {
            return iter_range.first->second;
        }
    }
    mapped_type operator[](key_type &&key) {
        auto iter_range = data_.equal_range(key);
        if (iter_range.first == iter_range.second) {
            auto iter =
                data_.insert_force(iter_range.first, value_type(std::move(key), mapped_type()));
            return iter->second;
        } else {
            return iter_range.first->second;
        }
    }

    template <class K> const_iterator find(const K &x) const { return data_.find(x); }
    template <class K> iterator find(const K &x) { return data_.find(x); }

    void clear() noexcept { data_.clear(); }

    size_type erase(const key_type &key) {
        auto iter_range = data_.equal_range(key);
        size_type ans = iter_range.second - iter_range.first;
        if (ans > 0) {
            data_.erase(iter_range.first, iter_range.second);
        }
        return ans;
    }

    iterator replace_key(const key_type &old_key, const key_type &new_key) {
        iterator iter = data_.lower_bound(old_key);
        iterator end_iter = data_.end();
        if (iter == end_iter || iter->first != old_key)
            return end_iter;
        if (old_key == new_key)
            return iter;

        auto &comp = data_.get_compare();
        if (comp(old_key, new_key)) {
            // circular shift [iter, pos) down
            auto pos = std::lower_bound(iter + 1, end_iter, new_key, comp);
            auto ans = data_.circ_shift(iter, pos, false);
            ans->first = new_key;
            return ans;
        } else {
            // circular shift [pos, iter) up
            auto pos = std::upper_bound(data_.begin(), iter, new_key, comp);
            auto ans = data_.circ_shift(pos, iter, true);
            ans->first = new_key;
            return ans;
        }
    }
};

} // namespace util
} // namespace cbag

#endif
