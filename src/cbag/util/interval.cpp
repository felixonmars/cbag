
#include <algorithm>

#include <cbag/util/interval.h>

namespace cbag {
namespace util {

bool intv_comp::operator()(value_type lhs, const interval_type &rhs) const {
    return lhs < rhs.first;
}

bool intv_comp::operator()(const interval_type &lhs, value_type rhs) const {
    return lhs.second <= rhs;
}

bool intv_comp::operator()(const interval_type &lhs, const interval_type &rhs) const {
    return lhs.second <= rhs.first;
}

dist_intvs::dist_intvs() = default;

dist_intvs::dist_intvs(map_type &&table) : table(std::move(table)) {}

dist_intvs::const_iterator dist_intvs::begin() const { return table.begin(); }

dist_intvs::const_iterator dist_intvs::end() const { return table.end(); }

bool dist_intvs::empty() const { return table.empty(); }

std::size_t dist_intvs::size() const { return table.size(); }

dist_intvs::coord_t dist_intvs::start() const {
    const auto iter = table.begin();
    if (iter == table.end())
        throw std::out_of_range("Cannot get start of empty interval.");
    return iter->first.first;
}

dist_intvs::coord_t dist_intvs::stop() const {
    const auto iter = table.rbegin();
    if (iter == table.rend())
        throw std::out_of_range("Cannot get stop of empty interval.");
    return iter->first.second;
}

dist_intvs::const_iterator dist_intvs::find_exact(const key_type &key) const {
    auto iter = table.find(key);
    auto end_iter = table.end();
    if (iter != end_iter && iter->first == key)
        return iter;
    return end_iter;
}

bool dist_intvs::contains(const key_type &key) const { return find_exact(key) != table.end(); }

bool dist_intvs::overlaps(const key_type &key) const { return table.find(key) != table.end(); }

bool dist_intvs::covers(const key_type &key) const {
    auto eq_range = table.equal_range(key);
    return eq_range.first != table.end() && ((++(eq_range.first)) == eq_range.second);
}

dist_intvs::map_type intersect_helper(const dist_intvs::map_type &t1,
                                      const dist_intvs::map_type &t2) {
    dist_intvs::map_type ans;
    for (const auto &pv : t1) {
        auto intv1 = pv.first;
        auto iter_pair = t2.equal_range(intv1);
        for (; iter_pair.first != iter_pair.second; ++(iter_pair.first)) {
            ans.emplace(std::make_pair(std::max(intv1.first, iter_pair.first->first.first),
                                       std::min(intv1.second, iter_pair.first->first.second)),
                        nullptr);
        }
    }
    return ans;
}

dist_intvs dist_intvs::get_intersect(const dist_intvs &other) const {
    if (size() < other.size())
        return dist_intvs(intersect_helper(this->table, other.table));
    return dist_intvs(intersect_helper(other.table, this->table));
}

dist_intvs dist_intvs::get_complement(coord_t lower, coord_t upper) const {
    map_type ans;
    if (table.empty()) {
        ans.emplace(std::make_pair(lower, upper), nullptr);
    } else {
        coord_t a = start();
        coord_t b = stop();
        if (a < lower || upper < b) {
            throw std::out_of_range(
                fmt::format("dist_intvs interval [{:d}, {:d}) not covered by [{:d}, {:d})", a, b,
                            lower, upper));
        }
        for (const auto &pair : table) {
            if (lower < pair.first.first)
                ans.emplace(std::make_pair(lower, pair.first.first), nullptr);
            lower = pair.first.second;
        }
        if (lower < upper)
            ans.emplace(std::make_pair(lower, upper), nullptr);
    }
    return {std::move(ans)};
}

bool dist_intvs::remove(const key_type &key) {
    auto iter = find_exact(key);
    if (iter == table.end()) {
        return false;
    }
    table.erase(iter);
    return true;
}

bool dist_intvs::remove_overlaps(const key_type &key) {
    auto iter_pair = table.equal_range(key);
    if (iter_pair.first == table.end())
        return false;
    table.erase(iter_pair.first, iter_pair.second);
    return true;
}

bool dist_intvs::add(key_type key, value_pointer value, bool merge, bool abut) { return true; }

} // namespace util
} // namespace cbag
