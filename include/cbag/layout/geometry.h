#ifndef CBAG_LAYOUT_GEOMETRY_H
#define CBAG_LAYOUT_GEOMETRY_H

#include <cstdint>
#include <variant>
#include <vector>

#include <boost/geometry/index/rtree.hpp>

#include <cbag/layout/geo_object.h>
#include <cbag/layout/polygon45_fwd.h>
#include <cbag/layout/polygon45_set.h>
#include <cbag/layout/polygon90_fwd.h>
#include <cbag/layout/polygon90_set.h>
#include <cbag/layout/polygon_fwd.h>
#include <cbag/layout/polygon_set.h>
#include <cbag/layout/pt_vector_fwd.h>
#include <cbag/util/overload.h>

namespace bgi = boost::geometry::index;

namespace cbag {
namespace layout {

class rectangle;

using geometry_index = bgi::rtree<geo_object, bgi::quadratic<32, 16>>;

/** A class representing layout geometries on the same layer.
 */
class geometry {
  private:
    using geometry_data = std::variant<polygon90_set, polygon45_set, polygon_set>;

    uint8_t mode = 0;
    geometry_data data;
    geometry_index index;
    struct helper;

  public:
    explicit geometry(uint8_t mode = 0);

    rectangle get_bbox() const;

    void reset_to_mode(uint8_t m);

    void add_shape(const rectangle &obj, offset_t spx, offset_t spy);
    void add_shape(const polygon90 &obj, offset_t spx, offset_t spy);
    void add_shape(const polygon45 &obj, offset_t spx, offset_t spy);
    void add_shape(const polygon45_set &obj, offset_t spx, offset_t spy);
    void add_shape(const polygon &obj, offset_t spx, offset_t spy);

    template <typename T> void write_geometry(T &output) const {
        std::visit(
            overload{
                [&](const polygon90_set &d) { d.get(output); },
                [&](const polygon45_set &d) { d.get(output); },
                [&](const polygon_set &d) { d.get(output); },
            },
            data);
    }

    static polygon45_set make_path(const pt_vector &data, offset_t half_width, uint8_t style0,
                                   uint8_t style1, uint8_t stylem);

    static polygon45_set make_path45_bus(const pt_vector &data, const std::vector<offset_t> &widths,
                                         const std::vector<offset_t> &spaces, uint8_t style0,
                                         uint8_t style1, uint8_t stylem);
};

} // namespace layout
} // namespace cbag

#endif
