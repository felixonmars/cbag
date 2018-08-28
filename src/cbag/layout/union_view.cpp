#include <cbag/layout/polygon45_view.h>
#include <cbag/layout/polygon90_view.h>
#include <cbag/layout/polygon_view.h>
#include <cbag/layout/rectangle.h>
#include <cbag/layout/rectangle_view.h>
#include <cbag/layout/union_view.h>

namespace cbag {
namespace layout {

class extents_visitor {
  public:
    rectangle &bbox;

    explicit extents_visitor(rectangle &bbox) : bbox(bbox) {}

    template <typename V> void operator()(const V &view) { bp::extents(bbox, view); }
};

rectangle extents(const union_view &view) {
    rectangle ans;
    std::visit(extents_visitor(ans), view);
    return ans;
}

} // namespace layout
} // namespace cbag
