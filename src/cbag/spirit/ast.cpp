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

#include <fmt/core.h>

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

name_rep::name_rep() = default;

uint32_t name_rep::size() const { return mult * std::max(idx_range.size(), 1u); }

bool name_rep::is_vector() const { return idx_range.size() > 0; }

std::string name_rep::operator[](uint32_t index) const {
    uint32_t range_size = idx_range.size();
    return (range_size == 0) ? base : fmt::format("{}<{}>", base, idx_range[index % range_size]);
}

bool name_rep::operator==(const name_rep &other) const {
    return base == other.base && idx_range == other.idx_range && mult == other.mult;
}

bool name_rep::operator!=(const name_rep &other) const { return !(*this == other); }

bool name_rep::operator<(const name_rep &other) const {
    return base < other.base ||
           (base == other.base &&
            (idx_range < other.idx_range || (idx_range == other.idx_range && mult < other.mult)));
}

name::const_iterator::const_iterator(const name *ptr, unsigned long unit_index, uint32_t bit_index)
    : ptr(ptr), unit_index(unit_index), bit_index(bit_index) {}

name::const_iterator &name::const_iterator::operator++() {
    if (bit_index < (ptr->rep_list[unit_index]).size() - 1) {
        ++bit_index;
    } else {
        ++unit_index;
        bit_index = 0;
    }

    return *this;
}

name::name() = default;

bool name::const_iterator::operator!=(const const_iterator &other) const {
    return ptr != other.ptr || unit_index != other.unit_index || bit_index != other.bit_index;
}

bool name::const_iterator::operator==(const const_iterator &other) const {
    return ptr == other.ptr && unit_index == other.unit_index && bit_index == other.bit_index;
}

std::string name::const_iterator::operator*() const { return ptr->rep_list[unit_index][bit_index]; }

name::const_iterator name::begin() const { return {this, 0, 0}; }

name::const_iterator name::end() const { return {this, rep_list.size(), 0}; }

uint32_t name::size() const {
    uint32_t tot = 0;
    for (auto const &nu : rep_list) {
        tot += nu.size();
    }
    return tot;
}

bool name::operator==(const name &other) const {
    unsigned long size1 = rep_list.size();
    unsigned long size2 = other.rep_list.size();

    if (size1 == size2) {
        for (unsigned long idx = 0; idx < size1; idx++) {
            if (rep_list[idx] != other.rep_list[idx]) {
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
    unsigned long size1 = rep_list.size();
    unsigned long size2 = other.rep_list.size();

    if (size1 < size2) {
        return true;
    } else if (size1 == size2) {
        for (unsigned long idx = 0; idx < size1; idx++) {
            if (rep_list[idx] < other.rep_list[idx]) {
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
