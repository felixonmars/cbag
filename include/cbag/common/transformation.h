
#ifndef CBAG_COMMON_TRANSFORM_H
#define CBAG_COMMON_TRANSFORM_H

#include <cbag/common/orientation.h>
#include <cbag/common/typedefs.h>

namespace bp = boost::polygon;

namespace cbag {

/** Our own geometry transformation class.
 *
 *  We use our own transformation class because boost::polygon transformation
 *  performs x' = M(x - p), where in BAG we use x' = Mx + p.  This class
 *  handles the conversion.
 */
class transformation : public bp::transformation<coord_t> {
  public:
    explicit transformation(coord_t dx = 0, coord_t dy = 0,
                            uint32_t mode = static_cast<uint32_t>(oR0));
    transformation(coord_t dx, coord_t dy, orientation orient);

    coord_t x() const;
    coord_t y() const;
    orientation orient() const;
    uint32_t orient_code() const;
    void get_location(coord_t &x, coord_t &y) const;
    bool flips_xy() const;

    transformation get_move_by(offset_t dx, offset_t dy) const;
    transformation get_inverse() const;

    void set_location(coord_t x, coord_t y);
    void set_orient_code(uint32_t code);
    void move_by(offset_t dx, offset_t dy);
};

} // namespace cbag

#endif
