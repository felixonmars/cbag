/** \file ast.cpp
 *  \brief This file implements various abstract syntax tree (AST) structures
 * parsed by Spirit X3.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <algorithm>
#include <cstdint>
#include <limits>
#include <vector>

#include <boost/functional/hash.hpp>

#include <fmt/format.h>

#include <cbag/spirit/ast.h>

namespace cbag {
namespace spirit {
namespace ast {
range::range() {}

range::range(uint32_t start, uint32_t stop, uint32_t step) : start(start), stop(stop), step(step) {}

uint32_t range::size() const {
    if (step == 0)
        return 0;
    if (stop >= start) {
        return (stop - start + step) / step;
    } else {
        return (start - stop + step) / step;
    }
}

uint32_t range::get_stop_exclude() const {
    if (stop >= start) {
        return start + size() * step;
    } else {
        return start - size() * step;
    }
}

uint32_t range::operator[](uint32_t index) const {
    return (stop >= start) ? start + step * index : start - step * index;
}

bool range::operator==(const range &other) const {
    return start == other.start && stop == other.stop && step == other.step;
}

bool range::operator!=(const range &other) const { return !(*this == other); }

bool range::operator<(const range &other) const {
    return start < other.start ||
           (start == other.start &&
            (stop < other.stop || (stop == other.stop && step < other.step)));
}

name_bit::name_bit() {}

name_bit::name_bit(std::string base) : base(std::move(base)) {}

name_bit::name_bit(std::string base, uint32_t index) : base(std::move(base)), index(index) {}

bool name_bit::operator==(const name_bit &other) const {
    return base == other.base && index == other.index;
}

bool name_bit::operator!=(const name_bit &other) const { return !(*this == other); }

bool name_bit::operator<(const name_bit &other) const {
    if (base < other.base) {
        return true;
    } else if (base == other.base) {
        if (index) {
            return bool(other.index) && (*index < *other.index);
        } else {
            return bool(other.index);
        }
    } else {
        return false;
    }
}

name_unit::name_unit() {}

uint32_t name_unit::size() const { return mult * std::max(idx_range.size(), 1u); }

bool name_unit::is_vector() const { return idx_range.size() > 0; }

name_bit name_unit::operator[](uint32_t index) const {
    uint32_t range_size = idx_range.size();
    return (range_size == 0) ? name_bit(base) : name_bit(base, idx_range[index % range_size]);
}

bool name_unit::operator==(const name_unit &other) const {
    return base == other.base && idx_range == other.idx_range && mult == other.mult;
}

bool name_unit::operator!=(const name_unit &other) const { return !(*this == other); }

bool name_unit::operator<(const name_unit &other) const {
    return base < other.base ||
           (base == other.base &&
            (idx_range < other.idx_range || (idx_range == other.idx_range && mult < other.mult)));
}

name::const_iterator::const_iterator(const name *ptr, unsigned long unit_index, uint32_t bit_index)
    : ptr(ptr), unit_index(unit_index), bit_index(bit_index) {}

name::const_iterator &name::const_iterator::operator++() {
    if (bit_index < (ptr->unit_list[unit_index]).size() - 1) {
        ++bit_index;
    } else {
        ++unit_index;
        bit_index = 0;
    }

    return *this;
}

name::name() {}

bool name::const_iterator::operator!=(const const_iterator &other) const {
    return ptr != other.ptr || unit_index != other.unit_index || bit_index != other.bit_index;
}

bool name::const_iterator::operator==(const const_iterator &other) const {
    return ptr == other.ptr && unit_index == other.unit_index && bit_index == other.bit_index;
}

name_bit name::const_iterator::operator*() const { return ptr->unit_list[unit_index][bit_index]; }

name::const_iterator name::begin() const { return {this, 0, 0}; }

name::const_iterator name::end() const { return {this, unit_list.size(), 0}; }

uint32_t name::size() const {
    uint32_t tot = 0;
    for (auto const &nu : unit_list) {
        tot += nu.size();
    }
    return tot;
}

bool name::operator==(const name &other) const {
    unsigned long size1 = unit_list.size();
    unsigned long size2 = other.unit_list.size();

    if (size1 == size2) {
        for (unsigned long idx = 0; idx < size1; idx++) {
            if (unit_list[idx] != other.unit_list[idx]) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

bool name::operator!=(const name &other) const { return !(*this == other); }

bool name::operator<(const name &other) const {
    unsigned long size1 = unit_list.size();
    unsigned long size2 = other.unit_list.size();

    if (size1 < size2) {
        return true;
    } else if (size1 == size2) {
        for (unsigned long idx = 0; idx < size1; idx++) {
            if (unit_list[idx] < other.unit_list[idx]) {
                return true;
            }
        }
        return false;
    } else {
        return false;
    }
}

} // namespace ast
} // namespace spirit
} // namespace cbag

namespace std {
// define hash function for NameUnit
template <> struct hash<cbag::spirit::ast::name_bit> {
    size_t operator()(const cbag::spirit::ast::name_bit &v) const {

        size_t seed = 0;
        boost::hash_combine(seed, v.base);
        boost::hash_combine(seed, (v.index) ? *(v.index) : std::numeric_limits<std::size_t>::max());

        return seed;
    }
};

} // namespace std
