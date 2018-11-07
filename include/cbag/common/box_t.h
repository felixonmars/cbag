
#ifndef CBAG_COMMON_BBOX_H
#define CBAG_COMMON_BBOX_H

#include <array>
#include <optional>

#include <boost/polygon/rectangle_concept.hpp>
#include <boost/polygon/rectangle_data.hpp>
#include <boost/polygon/rectangle_traits.hpp>

#include <cbag/common/typedefs.h>

namespace bp = boost::polygon;

namespace cbag {

class box_array_t;

class box_collection_t;

class transformation;

class box_t {
  public:
    using coordinate_type = coord_t;
    using interval_type = std::array<coord_t, 2>;

  private:
    interval_type intvs[2];

  public:
    box_t();

    box_t(interval_type horizontal, interval_type vertical);

    box_t(coord_t xl, coord_t yl, coord_t xh, coord_t yh);

    coord_t xl() const;
    coord_t yl() const;
    coord_t xh() const;
    coord_t yh() const;
    coord_t xm() const;
    coord_t ym() const;
    offset_t w() const;
    offset_t h() const;

    bool is_physical() const;
    bool is_valid() const;
    bool overlaps(const box_t &other) const;
    const interval_type &get_interval(bp::orientation_2d_enum orient) const;
    box_t get_merge(const box_t &other) const;
    box_t get_intersect(const box_t &other) const;
    box_t get_extend(const std::optional<coord_t> &x, const std::optional<coord_t> &y) const;
    box_t get_expand(coord_t dx = 0, coord_t dy = 0) const;
    box_t get_transform(const transformation &xform) const;
    box_t get_move_by(offset_t dx = 0, offset_t dy = 0) const;
    box_t get_flip_xy() const;
    box_t get_with_interval(bp::orientation_2d_enum orient, interval_type intv) const;
    box_array_t as_box_array() const;
    box_collection_t as_box_collection() const;

    void set(coord_t xl, coord_t yl, coord_t xh, coord_t yh);
    void set_interval(bp::orientation_2d_enum orient, interval_type interval);

    bool operator==(const box_t &other) const;
};

} // namespace cbag

namespace boost {
namespace polygon {

template <> struct geometry_concept<std::array<cbag::coord_t, 2>> {
    using type = interval_concept;
};

template <> struct interval_traits<std::array<cbag::coord_t, 2>> {
    using coordinate_type = cbag::coord_t;

    static coordinate_type get(const std::array<cbag::coord_t, 2> &interval, direction_1d dir) {
        return interval[dir.to_int()];
    }
};

template <> struct interval_mutable_traits<std::array<int, 2>> {
    using coordinate_type = cbag::coord_t;

    static void set(std::array<cbag::coord_t, 2> &interval, direction_1d dir,
                    coordinate_type value) {
        interval[dir.to_int()] = value;
    }
    static std::array<cbag::coord_t, 2> construct(coordinate_type low_value,
                                                  coordinate_type high_value) {
        return {low_value, high_value};
    }
};

template <> struct geometry_concept<cbag::box_t> { using type = rectangle_concept; };

template <> struct rectangle_traits<cbag::box_t> {
    typedef cbag::box_t::coordinate_type coordinate_type;
    typedef cbag::box_t::interval_type interval_type;
    static interval_type get(const cbag::box_t &rectangle, orientation_2d orient) {
        return rectangle.get_interval(static_cast<bp::orientation_2d_enum>(orient.to_int()));
    }
};

template <> struct rectangle_mutable_traits<cbag::box_t> {
    static inline void set(cbag::box_t &rectangle, orientation_2d orient,
                           const cbag::box_t::interval_type &interval) {
        rectangle.set_interval(static_cast<bp::orientation_2d_enum>(orient.to_int()), interval);
    }
    static inline cbag::box_t construct(const cbag::box_t::interval_type &interval_horizontal,
                                        const cbag::box_t::interval_type &interval_vertical) {
        return {interval_horizontal, interval_vertical};
    }
};

} // namespace polygon
} // namespace boost

#endif
