#ifndef CBAG_LAYOUT_GEOMETRY_H
#define CBAG_LAYOUT_GEOMETRY_H

#include <cstdint>
#include <variant>
#include <vector>

#include <cbag/common/layer_t.h>
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
class routing_grid;

/** A class representing layout geometries on the same layer.
 */
class geometry {
  private:
    using geometry_data = std::variant<polygon90_set, polygon45_set, polygon_set>;

    geometry_mode mode;
    geometry_data data;
    struct helper;

  public:
    explicit geometry(geometry_mode mode = geometry_mode::POLY90);

    bool operator==(const geometry &rhs) const noexcept;

    box_t get_bbox() const;

    void reset_to_mode(geometry_mode m);

    void add_shape(const box_t &obj);
    void add_shape(const polygon90 &obj);
    void add_shape(const polygon45 &obj);
    void add_shape(const polygon &obj);
    void add_shape(const polygon45_set &obj);

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
