#ifndef CBAG_UTIL_DIST_INTVS_H
#define CBAG_UTIL_DIST_INTVS_H

#include <map>
#include <type_traits>
#include <utility>

#include <fmt/format.h>

namespace cbag {
namespace util {

template <typename T, typename std::enable_if_t<std::is_integral_v<T>> * = nullptr>
struct intv_comp {
    using is_transparent = T;
    using intv_t = std::pair<T, T>;

    bool operator()(T lhs, const intv_t &rhs) const { return lhs < rhs.first; }
    bool operator()(const intv_t &lhs, T rhs) const { return lhs.second <= rhs; }
    bool operator()(const intv_t &lhs, const intv_t &rhs) const { return lhs.second <= rhs.first; }
};

template <typename T, typename V, typename std::enable_if_t<std::is_integral_v<T>> * = nullptr>
class dist_intvs {
  private:
    using intv_t = std::pair<T, T>;
    using intv_map_t = std::map<intv_t, V, intv_comp<T>>;

    intv_map_t table;

  public:
    dist_intvs() = default;

    bool contains(const intv_t &intv) const {
        auto iter = table.find(intv);
        return (iter != table.end()) && iter->first == intv;
    }

    typename intv_map_t::const_iterator begin() const { return table.begin(); }

    typename intv_map_t::const_iterator end() const { return table.end(); }

    V &at(const intv_t &intv) {
        auto iter = table.find(intv);
        if (iter == table.end() || intv != iter->first) {
            throw std::out_of_range(
                fmt::format("Cannot find interval [{}, {})", intv.first, intv.second));
        }
        return iter->second;
    }

    void set(const intv_t &intv, const V &val) {
        auto result = table.try_emplace(intv, val);
        if (!result.second) {
            if (result.first->first == intv) {
                result.first->second = val;
            } else {
                throw std::out_of_range(
                    fmt::format("Cannot find interval [{}, {})", intv.first, intv.second));
            }
        }
    }

    bool insert(intv_t intv, V val) {
        return table.try_emplace(std::move(intv), std::move(val)).second;
    }
};

} // namespace util
} // namespace cbag

#endif
