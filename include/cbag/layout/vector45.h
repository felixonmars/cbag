#ifndef CBAG_LAYOUT_VECTOR45_H
#define CBAG_LAYOUT_VECTOR45_H

#include <cbag/common/typedefs.h>
#include <cbag/layout/polygon.h>

namespace cbag {
namespace layout {

/** A POD-type that represents vectors with angles being multiples of 45 degrees.
 *
 *  As this is a POD type, it does not check if this is actually a valid vector.
 *  The norm of a 45 degree vector has length sqrt(2).
 */
struct vector45 {
    offset_t dx;
    offset_t dy;

    vector45 get_rotate90() const;
    vector45 get_norm() const;
    bool valid() const;
    /** Return true if vector is 45 degree or invalid.
     */
    bool is_45_or_invalid() const;
    void invert();

    friend std::ostream &operator<<(std::ostream &os, const vector45 &d);
};

} // namespace layout
} // namespace cbag

#endif
