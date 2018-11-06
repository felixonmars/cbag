#include <algorithm>

#include <cbag/layout/geo_iterator.h>
#include <cbag/util/overload.h>

using namespace boost::polygon::operators;

namespace cbag {
namespace layout {

struct geo_iterator::helper {

    struct xform_visitor {
      public:
        const cbag::transformation &xform;

        xform_visitor(const cbag::transformation &xform) : xform(xform) {}

        template <typename T> void operator()(T &v) { bp::transform(v, xform); }
    };

    struct geo_visitor {
      public:
        geo_iterator &self;

        geo_visitor(geo_iterator &self) : self(self) {}

        bool operator()(const geo_instance &v) {
            self.inner =
                std::make_shared<geo_iterator>(v.begin_intersect(self.box, self.spx, self.spy));
            return true;
        }

        template <typename T> bool operator()(const T &v) {
            rectangle test_box = self.box.get_expand(std::max(self.spx, self.cur->spx),
                                                     std::max(self.spy, self.cur->spy));
            if (bp::empty(v & test_box)) {
                ++self.cur;
                return true;
            }
            self.cur_val = geo_union(v);
            std::visit(xform_visitor(self.xform), self.cur_val.val);
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
                if (self.inner->has_next()) {
                    // get inner item
                    self.cur_val = *(*self.inner);
                    std::visit(xform_visitor(self.xform), self.cur_val.val);
                    return;
                } else {
                    // no more inner items
                    self.inner.reset();
                    ++self.cur;
                }
            }
        }
    }
};

geo_iterator::geo_iterator() = default;

geo_iterator::geo_iterator(const rectangle &box, offset_t spx, offset_t spy, geo_query_iter &&cur,
                           geo_query_iter &&end, const cbag::transformation &xform)
    : box(box), spx(spx), spy(spy), cur(std::move(cur)), end(std::move(end)), xform(xform) {
    helper::get_val_reference(*this);
}

bool geo_iterator::has_next() const { return cur != end; }

geo_iterator &geo_iterator::operator++() {
    if (inner == nullptr) {
        ++cur;
    } else {
        ++(*inner);
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
    bool a = has_next();
    bool b = rhs.has_next();
    return (!a && !b) ||
           (a && b && box == rhs.box && spx == rhs.spx && spy == rhs.spy && cur == rhs.cur &&
            end == rhs.end && xform == rhs.xform && inner == rhs.inner);
}

bool geo_iterator::operator!=(const geo_iterator &rhs) const { return !(*this == rhs); }

} // namespace layout
} // namespace cbag
