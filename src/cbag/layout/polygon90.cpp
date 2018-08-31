#include <cbag/layout/polygon90.h>

namespace cbag {
namespace layout {

namespace poly90_iter {

compact_iterator::compact_iterator() = default;

compact_iterator::compact_iterator(pt_vector::const_iterator iter, uint8_t parity)
    : iter(std::move(iter)), parity(parity) {}

compact_iterator &compact_iterator::operator+=(difference_type rhs) {
    rhs += parity;
    iter += rhs >> 1;
    parity = rhs & 1;
    return *this;
}

compact_iterator &compact_iterator::operator-=(difference_type rhs) {
    rhs = -rhs + parity;
    iter += rhs >> 1;
    parity = rhs & 1;
    return *this;
}

const compact_iterator::value_type compact_iterator::operator*() const { return (*iter)[parity]; }

compact_iterator::pointer compact_iterator::operator->() const { return iter->get_val_ptr(parity); }

const compact_iterator::value_type compact_iterator::operator[](difference_type rhs) const {
    rhs += parity;
    return iter[rhs >> 1][rhs & 1];
}

compact_iterator &compact_iterator::operator++() {
    iter += parity;
    parity ^= 1;
    return *this;
}

compact_iterator &compact_iterator::operator--() {
    iter -= 1 - parity;
    parity ^= 1;
    return *this;
}
compact_iterator compact_iterator::operator++(int) {
    compact_iterator tmp(iter, parity);
    ++(*this);
    return tmp;
}

compact_iterator compact_iterator::operator--(int) {
    compact_iterator tmp(iter, parity);
    --(*this);
    return tmp;
}

compact_iterator::difference_type compact_iterator::operator-(const compact_iterator &rhs) const {
    return ((iter - rhs.iter) << 1) + (parity - rhs.parity);
}

compact_iterator compact_iterator::operator+(difference_type rhs) const {
    rhs += parity;
    return {iter + (rhs >> 1), (uint8_t)(rhs & 1)};
}

compact_iterator compact_iterator::operator-(difference_type rhs) const {
    rhs = rhs - parity;
    return {iter + (rhs >> 1), (uint8_t)(rhs & 1)};
}

compact_iterator operator+(compact_iterator::difference_type lhs, const compact_iterator &rhs) {
    lhs += rhs.parity;
    return {rhs.iter + (lhs >> 1), (uint8_t)(lhs & 1)};
}

bool compact_iterator::operator==(const compact_iterator &rhs) const {
    return iter == rhs.iter && parity == rhs.parity;
}

bool compact_iterator::operator!=(const compact_iterator &rhs) const {
    return iter != rhs.iter || parity != rhs.parity;
}

bool compact_iterator::operator>(const compact_iterator &rhs) const {
    return iter > rhs.iter || (iter == rhs.iter && parity > rhs.parity);
}

bool compact_iterator::operator<(const compact_iterator &rhs) const { return rhs > (*this); }

bool compact_iterator::operator>=(const compact_iterator &rhs) const { return !((*this) < rhs); }

bool compact_iterator::operator<=(const compact_iterator &rhs) const { return !((*this) > rhs); }

point_iterator::point_iterator() = default;

point_iterator::point_iterator(pt_vector::const_iterator start, pt_vector::size_type size,
                               uint64_t idx)
    : start(std::move(start)), size(size), idx(idx) {
    set_point();
}

void point_iterator::set_point() {
    if (idx & 1) {
        pt_vector::size_type tmp = idx >> 1;
        if (tmp == size - 1) {
            pt.set(start->x(), start[tmp].y());
        } else {
            pt.set(start[tmp + 1].x(), start[tmp].y());
        }
    }
}

point_iterator &point_iterator::operator+=(difference_type rhs) {
    idx += rhs;
    set_point();
    return *this;
}

point_iterator &point_iterator::operator-=(difference_type rhs) {
    idx -= rhs;
    set_point();
    return *this;
}

point_iterator::reference point_iterator::operator*() const {
    if (idx & 1) {
        return pt;
    } else {
        return start[idx >> 1];
    }
}

const point_iterator::value_type point_iterator::operator[](difference_type rhs) const {
    uint64_t tmp = idx + rhs;
    if (tmp & 1) {
        tmp >>= 1;
        if (tmp == size - 1) {
            return point_t::create(start->x(), start[tmp].y());
        } else {
            return point_t::create(start[tmp + 1].x(), start[tmp].y());
        }
    } else {
        return start[tmp >> 1];
    }
}

point_iterator &point_iterator::operator++() {
    ++idx;
    set_point();
    return *this;
}

point_iterator &point_iterator::operator--() {
    --idx;
    set_point();
    return *this;
}
point_iterator point_iterator::operator++(int) {
    point_iterator tmp(start, size, idx);
    ++(*this);
    return tmp;
}

point_iterator point_iterator::operator--(int) {
    point_iterator tmp(start, size, idx);
    --(*this);
    return tmp;
}

point_iterator::difference_type point_iterator::operator-(const point_iterator &rhs) const {
    return idx - rhs.idx;
}

point_iterator point_iterator::operator+(difference_type rhs) const {
    return {start, size, idx + rhs};
}

point_iterator point_iterator::operator-(difference_type rhs) const {
    return {start, size, idx - rhs};
}

point_iterator operator+(point_iterator::difference_type lhs, const point_iterator &rhs) {
    return {rhs.start, rhs.size, rhs.idx + lhs};
}

bool point_iterator::operator==(const point_iterator &rhs) const {
    return start == rhs.start && idx == rhs.idx;
}

bool point_iterator::operator!=(const point_iterator &rhs) const {
    return start != rhs.start || idx != rhs.idx;
}

bool point_iterator::operator>(const point_iterator &rhs) const { return idx > rhs.idx; }

bool point_iterator::operator<(const point_iterator &rhs) const { return idx < rhs.idx; }

bool point_iterator::operator>=(const point_iterator &rhs) const { return idx >= rhs.idx; }

bool point_iterator::operator<=(const point_iterator &rhs) const { return idx <= rhs.idx; }

} // namespace poly90_iter

polygon90::compact_iterator_type polygon90::begin_compact() const { return {data.begin(), 0}; }

polygon90::compact_iterator_type polygon90::end_compact() const { return {data.end(), 0}; }

polygon90::iterator_type polygon90::begin() const { return {data.begin(), data.size(), 0}; }

polygon90::iterator_type polygon90::end() const {
    pt_vector::size_type size = data.size();
    return {data.begin(), size, ((uint64_t)size) << 1};
}

} // namespace layout
} // namespace cbag
