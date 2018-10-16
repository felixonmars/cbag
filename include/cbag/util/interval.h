#ifndef CBAG_UTIL_INTERVAL_H
#define CBAG_UTIL_INTERVAL_H

#include <map>
#include <type_traits>
#include <utility>

#include <fmt/format.h>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace util {

struct intv_comp {
    using is_transparent = offset_t;
    using value_type = offset_t;
    using interval_type = std::pair<value_type, value_type>;

    bool operator()(value_type lhs, const interval_type &rhs) const;
    bool operator()(const interval_type &lhs, value_type rhs) const;
    bool operator()(const interval_type &lhs, const interval_type &rhs) const;
};

class dist_intvs {
  public:
    using coord_t = offset_t;
    using key_type = std::pair<coord_t, coord_t>;
    using value_pointer = void *;
    using map_type = std::map<key_type, value_pointer, intv_comp>;
    using const_iterator = map_type::const_iterator;

  private:
    map_type table;

  public:
    dist_intvs();

    dist_intvs(map_type &&table);

    const_iterator begin() const;

    const_iterator end() const;

    bool empty() const;

    std::size_t size() const;

    coord_t start() const;

    coord_t stop() const;

    const_iterator find_exact(const key_type &key) const;

    bool contains(const key_type &key) const;

    bool overlaps(const key_type &key) const;

    bool covers(const key_type &key) const;

    dist_intvs get_intersect(const dist_intvs &other) const;

    dist_intvs get_complement(coord_t lower, coord_t upper) const;

    bool remove(const key_type &key);

    bool remove_overlaps(const key_type &key);

    bool add(key_type key, value_pointer value = nullptr, bool merge = false, bool abut = false);
};

} // namespace util
} // namespace cbag

#endif
