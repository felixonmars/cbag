#ifndef CBAG_LAYOUT_TRANSFORMATION_H
#define CBAG_LAYOUT_TRANSFORMATION_H

#include <boost/polygon/polygon.hpp>

#include <cbag/common/transform.h>
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
    transformation(coord_t dx, coord_t dy, uint32_t mode);

    coord_t x() const;
    coord_t y() const;
    bp::axis_transformation::ATR orient() const;
    uint32_t orient_code() const;
    void get_location(coord_t &x, coord_t &y) const;
    cbag::transform to_transform() const;

    void set_location(coord_t x, coord_t y);
    void set_orient_code(uint32_t code);
    void move_by(offset_t dx, offset_t dy);
};

} // namespace layout
} // namespace cbag

#endif
