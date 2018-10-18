
#include <algorithm>
#include <array>

#include <fmt/format.h>

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

disjoint_intvs::disjoint_intvs() = default;

disjoint_intvs::disjoint_intvs(map_type &&table) : table(std::move(table)) {}

disjoint_intvs::const_iterator disjoint_intvs::begin() const { return table.begin(); }

disjoint_intvs::const_iterator disjoint_intvs::end() const { return table.end(); }

std::pair<disjoint_intvs::const_iterator, disjoint_intvs::const_iterator>
disjoint_intvs::overlap_range(const key_type &key) const {
    return table.equal_range(key);
}

bool disjoint_intvs::empty() const { return table.empty(); }

std::size_t disjoint_intvs::size() const { return table.size(); }

disjoint_intvs::coord_t disjoint_intvs::start() const { return table.at_front().first.first; }

disjoint_intvs::coord_t disjoint_intvs::stop() const { return table.at_back().first.second; }

disjoint_intvs::const_iterator disjoint_intvs::find_exact(const key_type &key) const {
    return table.find_exact(key);
}

bool disjoint_intvs::contains(const key_type &key) const { return find_exact(key) != table.end(); }

bool disjoint_intvs::overlaps(const key_type &key) const {
    auto iter_range = table.equal_range(key);
    return iter_range.first != iter_range.second;
}

bool disjoint_intvs::covers(const key_type &key) const { return table.equal_size(key) == 1; }

void intersect_helper(disjoint_intvs::map_type &ans, const disjoint_intvs::key_type &intv,
                      disjoint_intvs::const_iterator &first,
                      const disjoint_intvs::const_iterator &last,
                      const disjoint_intvs::map_type::compare_type &comp) {
    auto iter_pair = std::equal_range(first, last, intv, comp);
    first = (iter_pair.first == iter_pair.second) ? iter_pair.first : iter_pair.second - 1;
    for (; iter_pair.first != iter_pair.second; ++(iter_pair.first)) {
        coord_t start = std::max(intv.first, iter_pair.first->first.first);
        coord_t stop = std::min(intv.second, iter_pair.first->first.second);
        ans.emplace(std::make_pair(start, stop), nullptr);
    }
}

disjoint_intvs disjoint_intvs::get_intersect(const disjoint_intvs &other) const {
    auto iter1 = table.begin();
    auto iter2 = other.table.begin();
    auto end1 = table.end();
    auto end2 = other.table.end();
    const auto &comp = table.get_compare();

    disjoint_intvs::map_type ans;
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
    return {std::move(ans)};
}

disjoint_intvs disjoint_intvs::get_complement(coord_t lower, coord_t upper) const {
    map_type ans;
    if (table.empty()) {
        ans.emplace(std::make_pair(lower, upper), nullptr);
    } else {
        coord_t a = start();
        coord_t b = stop();
        if (a < lower || upper < b) {
            throw std::out_of_range(
                fmt::format("disjoint_intvs interval [{:d}, {:d}) not covered by [{:d}, {:d})", a,
                            b, lower, upper));
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

disjoint_intvs disjoint_intvs::get_transform(coord_t scale, coord_t shift) const {
    map_type new_table;

    if (scale > 0) {
        for (const auto &p : table) {
            new_table.emplace(
                key_type(scale * p.first.first + shift, scale * p.first.second + shift), p.second);
        }
    } else if (scale < 0) {
        for (const auto &p : table) {
            new_table.emplace(
                key_type(scale * p.first.second + shift, scale * p.first.first + shift), p.second);
        }
    }

    return {std::move(new_table)};
}

bool disjoint_intvs::remove(const key_type &key) {
    auto iter = find_exact(key);
    if (iter == table.end()) {
        return false;
    }
    table.erase(iter);
    return true;
}

bool disjoint_intvs::remove_overlaps(const key_type &key) {
    auto iter_pair = table.equal_range(key);
    if (iter_pair.first == table.end())
        return false;
    table.erase(iter_pair.first, iter_pair.second);
    return true;
}

bool disjoint_intvs::substract(const key_type &key) {
    auto iter_pair = table.equal_range(key);
    if (iter_pair.first == table.end())
        return false;

    // get intervals we need to add back
    uint8_t add_mode = 0;
    std::pair<key_type, value_pointer> val0;
    std::pair<key_type, value_pointer> val1;
    coord_t test = iter_pair.first->first.first;
    if (test < key.first) {
        val0.first = std::make_pair(test, key.first);
        val0.second = iter_pair.first->second;
        add_mode = 0b01;
    }
    auto last_iter = iter_pair.second;
    test = (--last_iter)->first.second;
    if (key.second < test) {
        val1.first = std::make_pair(key.second, test);
        val1.second = last_iter->second;
        add_mode |= 0b10;
    }

    // remove intervals
    table.erase(iter_pair.first, iter_pair.second);
    // add back intervals
    switch (add_mode) {
    case 0b01:
        table.insert(std::move(val0));
        break;
    case 0b10:
        table.insert(std::move(val1));
        break;
    case 0b11:
        table.insert(std::move(val0));
        table.insert(std::move(val1));
        break;
    default:
        break;
    }
    return true;
}

bool disjoint_intvs::add(const key_type &key, value_pointer value, bool merge, bool abut) {
    abut = abut && merge;
    auto iter_pair = (abut) ? table.equal_range(key_type(key.first - 1, key.second + 1))
                            : table.equal_range(key);
    if (iter_pair.first == table.end()) {
        // no overlapping or abutting intervals
        table.emplace(key, value);
        return true;
    } else if (merge) {
        // have overlapping/abutting intervals, and we want to merge
        auto last_iter = iter_pair.second;

        coord_t lower = std::min(key.first, iter_pair.first->first.first);
        coord_t upper = std::max(key.second, (--last_iter)->first.second);
        table.erase(iter_pair.first, iter_pair.second);
        table.emplace(key_type(lower, upper), value);
    }
    return false;
}

} // namespace util
} // namespace cbag
