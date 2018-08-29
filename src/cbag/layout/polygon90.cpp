#include <cbag/layout/polygon90.h>

namespace cbag {
namespace layout {

compact_iterator::compact_iterator() = default;

compact_iterator::compact_iterator(point_vector_t::const_iterator start,
                                   point_vector_t::const_iterator stop, bool second)
    : start(std::move(start)), stop(std::move(stop)), second(second) {}

compact_iterator &compact_iterator::operator++() {
    if (second) {
        ++start;
        second = false;
    } else {
        second = true;
    }
    return *this;
}

point_iterator::point_iterator() = default;

point_iterator::point_iterator(const point_vector_t *ptr, std::size_t idx, bool between)
    : ptr(ptr), idx(idx), between(between) {}

point_t point_iterator::operator*() const {
    if (between) {
        if (idx == ptr->size() - 1) {
            return point_t((*ptr)[0].x(), (*ptr)[idx].y());
        }
        return point_t((*ptr)[idx + 1].x(), (*ptr)[idx].y());
    }
    return (*ptr)[idx];
}

point_iterator &point_iterator::operator++() {
    if (between) {
        ++idx;
        between = false;
    } else {
        between = true;
    }
    return *this;
}

polygon90::polygon90() = default;

polygon90::polygon90(std::size_t n) : polygon45(n) {}

polygon90::polygon90(point_vector_t data, winding_dir wdir) : polygon45(std::move(data), wdir) {}

} // namespace layout
} // namespace cbag
