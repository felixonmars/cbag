#include <algorithm>

#include <cbag/layout/geo_iterator.h>
#include <cbag/util/overload.h>

using namespace boost::polygon::operators;

namespace cbag {
namespace layout {

struct geo_iterator::helper {

    struct xform_visitor {
      public:
        const transformation &xform;

        xform_visitor(const transformation &xform) : xform(xform) {}

        template <typename T> void operator()(T &v) { bp::transform(v, xform); }
    };

    struct geo_visitor {
      public:
        geo_iterator &self;

        geo_visitor(geo_iterator &self) : self(self) {}

        bool operator()(const geo_instance &v) {
            // TODO: set inner iterator
            return true;
        }

        template <typename T> bool operator()(const T &v) {
            rectangle test_box = self.box.get_expand(std::max(self.spx, self.cur->spx),
                                                     std::max(self.spy, self.cur->spy));
            if (bp::empty(v & test_box)) {
                ++self.cur;
                return true;
            }
            self.cur_val = v;
            std::visit(xform_visitor(self.xform), self.cur_val);
            return false;
        }
    };

    // advance the iterator to the next point that
    static void get_val_reference(geo_iterator &self) {
        for (bool repeat = true; repeat;) {
            if (self.inner == nullptr) {
                // no inner loop
                if (self.cur == self.end) {
                    return;
                } else {
                    // still has item, check intersection
                    repeat = std::visit(geo_visitor(self), self.cur->val);
                }
            } else {
                // has inner loop
                if (self.inner->first == self.inner->second) {
                    // no more inner items
                    self.inner.reset();
                    ++self.cur;
                } else {
                    // get inner item
                    self.cur_val = *(self.inner->first);
                    std::visit(xform_visitor(self.xform), self.cur_val);
                    return;
                }
            }
        }
    }
};

geo_iterator::geo_iterator() = default;

geo_iterator::geo_iterator(rectangle box, offset_t spx, offset_t spy, geo_query_iter &&cur,
                           geo_query_iter &&end, transformation &&xform)
    : box(std::move(box)), spx(spx), spy(spy), cur(std::move(cur)), end(std::move(end)),
      xform(std::move(xform)) {
    helper::get_val_reference(*this);
}

geo_iterator &geo_iterator::operator++() {
    if (inner == nullptr) {
        ++cur;
    } else {
        ++inner->first;
    }
    helper::get_val_reference(*this);
    return *this;
}

geo_iterator geo_iterator::operator++(int) {
    geo_iterator ans = *this;
    ++(*this);
    return ans;
}

geo_iterator::reference geo_iterator::operator*() const { return cur_val; }

bool geo_iterator::operator==(const geo_iterator &rhs) const {
    bool a = (cur == end);
    bool b = (rhs.cur == rhs.end);
    return (a && b) ||
           (!a && !b && box == rhs.box && spx == rhs.spx && spy == rhs.spy && cur == rhs.cur &&
            end == rhs.end && xform == rhs.xform && inner == rhs.inner);
}

bool geo_iterator::operator!=(const geo_iterator &rhs) const { return !(*this == rhs); }

} // namespace layout
} // namespace cbag
