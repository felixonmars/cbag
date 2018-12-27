#ifndef CBAG_LAYOUT_GEOMETRY_H
#define CBAG_LAYOUT_GEOMETRY_H

#include <cstdint>
#include <variant>
#include <vector>

#include <cbag/common/transformation.h>
#include <cbag/enum/geometry_mode.h>
#include <cbag/layout/geo_index.h>
#include <cbag/layout/polygon45_fwd.h>
#include <cbag/layout/polygon45_set_fwd.h>
#include <cbag/layout/polygon90_fwd.h>
#include <cbag/layout/polygon90_set_fwd.h>
#include <cbag/layout/polygon_fwd.h>
#include <cbag/layout/polygon_set.h>
#include <cbag/util/overload.h>

namespace bgi = boost::geometry::index;

namespace cbag {

class box_t;

namespace layout {

class tech;

/** A class representing layout geometries on the same layer.
 */
class geometry {
  private:
    using geometry_data = std::variant<polygon90_set, polygon45_set, polygon_set>;

    geometry_mode mode = geometry_mode::POLY90;
    geometry_data data;
    geo_index index;
    struct helper;

  public:
    geometry();

    geometry(std::string &&lay_type, tech *tech_ptr, geometry_mode mode = geometry_mode::POLY90);

    bool index_empty() const;
    box_t get_bbox() const;
    box_t get_index_bbox() const;

    geo_iterator begin_intersect(const box_t &r, offset_t spx, offset_t spy,
                                 const transformation &xform = transformation()) const;

    void reset_to_mode(geometry_mode m);

    void add_shape(const box_t &obj, bool is_horiz);
    void add_shape(const polygon90 &obj, bool is_horiz);
    void add_shape(const polygon45 &obj, bool is_horiz);
    void add_shape(const polygon45_set &obj, bool is_horiz);
    void add_shape(const polygon &obj, bool is_horiz);

    void record_instance(const geometry *master, transformation xform);

    template <typename T> void write_geometry(T &output) const {
        std::visit(
            overload{
                [&output](const polygon90_set &d) { d.get(output); },
                [&output](const polygon45_set &d) { d.get(output); },
                [&output](const polygon_set &d) { d.get(output); },
            },
            data);
    }
};

} // namespace layout
} // namespace cbag

#endif
