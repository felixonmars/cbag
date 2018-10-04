#include <algorithm>

#include <cbag/layout/geo_iterator.h>
#include <cbag/util/overload.h>

using namespace boost::polygon::operators;

namespace cbag {
namespace layout {

struct geo_iterator::helper {
    struct geo_visitor {
      public:
        enum rep_mode : uint8_t {
            GOOD = 0,
            INC_ITER = 1,
            SET_INNER = 2,
        };

        rectangle test_box;

        geo_visitor(rectangle &&box) : test_box(std::move(box)) {}

        rep_mode operator()(const geo_instance &v) { return SET_INNER; }

        template <typename T> rep_mode operator()(const T &v) {
            if (bp::empty(v & test_box)) {
                return INC_ITER;
            }
            return GOOD;
        }
    };

    static void step(geo_iterator &self) {
        for (auto mode = geo_visitor::rep_mode::INC_ITER; mode != geo_visitor::rep_mode::GOOD;) {
            if (self.inner == nullptr) {
                // no inner loop
                if (self.cur != self.end) {
                    // still has item, check intersection
                    mode = std::visit(
                        geo_visitor(self.box.get_expand(std::max(self.spx, self.cur->spx),
                                                        std::max(self.spy, self.cur->spy))),
                        self.cur->val);
                    switch (mode) {
                    case geo_visitor::rep_mode::INC_ITER:
                        ++self.cur;
                        break;
                    case geo_visitor::rep_mode::SET_INNER:
                        // TODO: set self.inner
                    default:
                        break;
                    }
                } else {
                    mode = geo_visitor::rep_mode::GOOD;
                }
            } else {
                // has inner loop
                if (self.inner->first == self.inner->second) {
                    // no more inner items
                    self.inner.reset();
                    ++self.cur;
                } else {
                    // check inner item
                    mode = std::visit(
                        geo_visitor(self.box.get_expand(std::max(self.spx, self.cur->spx),
                                                        std::max(self.spy, self.cur->spy))),
                        *(self.inner->first));
                    if (mode == geo_visitor::rep_mode::INC_ITER)
                        ++(self.inner->first);
                }
            }
        }
    }
};

geo_iterator::geo_iterator() = default;

geo_iterator::geo_iterator(rectangle box, offset_t spx, offset_t spy, geo_query_iter &&cur,
                           geo_query_iter &&end)
    : box(std::move(box)), spx(spx), spy(spy), cur(std::move(cur)), end(std::move(end)) {
    helper::step(*this);
}

geo_iterator &geo_iterator::operator++() {
    if (inner == nullptr) {
        ++cur;
    } else {
        ++inner->first;
    }
    helper::step(*this);
    return *this;
}

geo_iterator geo_iterator::operator++(int) {
    geo_iterator ans = *this;
    ++(*this);
    return ans;
}

geo_iterator::value_type geo_iterator::operator*() const {
    if (inner == nullptr)
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
                          cur->val);
    return *(inner->first);
}

bool geo_iterator::operator==(const geo_iterator &rhs) const {
    bool a = (cur == end);
    bool b = (rhs.cur == rhs.end);
    return (a && b) || (!a && !b && box == rhs.box && spx == rhs.spx && spy == rhs.spy &&
                        cur == rhs.cur && end == rhs.end && inner == rhs.inner);
}

bool geo_iterator::operator!=(const geo_iterator &rhs) const { return !(*this == rhs); }

} // namespace layout
} // namespace cbag
