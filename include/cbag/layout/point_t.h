#ifndef CBAG_LAYOUT_POINT_T_H
#define CBAG_LAYOUT_POINT_T_H

#include <cstdint>

#include <boost/polygon/polygon.hpp>
#include <cbag/common/typedefs.h>

namespace cbag {
namespace layout {

/** A POD 32-bit data structure represented as 4 bytes
 *
 * This class is used over 32-bit integers because it has 1-byte alignment.
 */
struct byte4_t {
    uint8_t data[4];

    explicit operator uint32_t() const;

    explicit operator int32_t() const;

    const byte4_t operator=(int32_t value);

    const byte4_t operator=(uint32_t value);

    static byte4_t convert(int32_t value);
};

/** A POD custom point type for boost polygon.
 *
 *  This class is special because it can be re-interpreted as a 64-bit pointer,
 *  and also return unsigned version of the two integers.  This is used to hack a
 *  space efficient point array.
 */
struct point_t {
    using coordinate_type = coord_t;

    byte4_t val[2];

    explicit operator uint64_t() const;

    explicit operator void *() const;

    explicit operator point_t *() const;

    explicit operator const point_t *() const;

    int32_t x() const;

    int32_t y() const;

    uint32_t ux() const;

    uint32_t uy() const;

    int32_t operator[](uint32_t idx) const;

    byte4_t &operator[](uint32_t idx);

    const int32_t *get_val_ptr(uint32_t idx) const;

    void set(uint32_t ux, uint32_t uy);

    void set(int32_t x, int32_t y);

    void set(void *ptr);

    static point_t create(coord_t x, coord_t y);
};

} // namespace layout
} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<cbag::layout::point_t> { using type = point_concept; };

// Then we specialize the gtl point traits for our point type
template <> struct point_traits<cbag::layout::point_t> {
    using coordinate_type = cbag::layout::point_t::coordinate_type;

    static inline coordinate_type get(const cbag::layout::point_t &point, orientation_2d orient) {
        return point[orient.to_int()];
    }
};

template <> struct point_mutable_traits<cbag::layout::point_t> {
    using coordinate_type = cbag::layout::point_t::coordinate_type;

    static inline void set(cbag::layout::point_t &point, orientation_2d orient,
                           coordinate_type value) {
        point[orient.to_int()] = value;
    }
    static inline cbag::layout::point_t construct(coordinate_type x_value,
                                                  coordinate_type y_value) {
        return cbag::layout::point_t::create(x_value, y_value);
    }
};
} // namespace polygon
} // namespace boost

#endif
