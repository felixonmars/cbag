#include <cbag/layout/geo_iterator.h>
#include <cbag/util/overload.h>

namespace cbag {
namespace layout {

geo_iterator::geo_iterator() = default;

geo_iterator::geo_iterator(geo_query_iter &&cur, bool step) : ptr(std::move(cur)) {
    if (step) {
        // TODO: do step
    }
}

geo_iterator &geo_iterator::operator++() {
    // TODO: do step
    return *this;
}

geo_iterator geo_iterator::operator++(int) {
    geo_iterator ans = *this;
    ++(*this);
    return ans;
}

geo_iterator::value_type geo_iterator::operator*() const {
    if (start != nullptr)
        return *(*start);
    return std::visit(overload{[&](const rectangle &v) { return flat_geo_type(v); },
                               [&](const polygon90 &v) { return flat_geo_type(v); },
                               [&](const polygon45 &v) { return flat_geo_type(v); },
                               [&](const polygon &v) { return flat_geo_type(v); },
                               [&](const geo_instance &v) {
                                   throw std::runtime_error(
                                       "geo_instance found in geo_iterator, see developer.");
                                   // unreachable; but used to infer lambda function return type
                                   return flat_geo_type();
                               }},
                      ptr->value());
}

bool geo_iterator::operator==(const geo_iterator &rhs) const {
    // TODO: fix this
    return true;
}

bool geo_iterator::operator!=(const geo_iterator &rhs) const { return !(*this == rhs); }

} // namespace layout
} // namespace cbag
