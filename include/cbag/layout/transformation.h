#ifndef CBAG_LAYOUT_TRANSFORMATION_H
#define CBAG_LAYOUT_TRANSFORMATION_H

#include <boost/polygon/polygon.hpp>

#include <cbag/common/typedefs.h>

namespace bp = boost::polygon;

namespace cbag {
namespace layout {

/** Our own geometry transformation class.
 *
 *  We use our own transformation class because boost::polygon transformation
 *  performs x' = M(x - p), where in BAG we use x' = Mx + p.  This class
 *  handles the conversion.
 */
class transformation : public bp::transformation<coord_t> {
  public:
    transformation();
    explicit transformation(bp::axis_transformation::ATR orient);
    transformation(coord_t dx, coord_t dy);
    transformation(coord_t dx, coord_t dy, bp::axis_transformation::ATR orient);
};

} // namespace layout
} // namespace cbag

#endif
