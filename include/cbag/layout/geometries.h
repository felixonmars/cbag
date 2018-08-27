#ifndef CBAG_LAYOUT_GEOMETRIES_H
#define CBAG_LAYOUT_GEOMETRIES_H

#include <algorithm>
#include <unordered_map>
#include <utility>
#include <variant>

#include <boost/functional/hash.hpp>

#include <cbag/layout/datatypes.h>

namespace cbag {
namespace layout {

using layer_t = std::pair<lay_t, purp_t>;

// custom unsigned char literal definition
inline constexpr unsigned char operator"" _uc(unsigned long long arg) noexcept {
    return static_cast<unsigned char>(arg);
}

// -----------------------------------------------------------------------------
// union views variant declaration
// -----------------------------------------------------------------------------

using UnionViewVariant = std::variant<RectView, Poly90View, Poly45View, PolyView>;

template <typename T> class PolyRef {
  public:
    inline PolyRef() : vec(nullptr), idx(0) {}

    inline PolyRef(std::vector<T> *vec, std::size_t idx) : vec(vec), idx(idx) {}

    inline T &value() { return (*vec)[idx]; }

  private:
    std::vector<T> *vec;
    std::size_t idx;
};

/** A class representing layout geometries on the same layer.
 */
class Geometry {
  public:
    inline explicit Geometry(uint8_t mode = 0)
        : rect_set(), poly90_set(), poly45_set(), poly_set(), mode(mode), view(make_union_view()) {}

    inline void set_mode(uint8_t m) { mode = m; }

    inline const UnionViewVariant &get_view() { return view; }

    inline PolyRef<Rect> add_rect(coord_t xl, coord_t yl, coord_t xh, coord_t yh) {
        rect_set.emplace_back(xl, yl, xh, yh);
        return {&rect_set, rect_set.size() - 1};
    }

    inline PolyRef<Polygon90> add_poly90(point_vector_t &&data) {
        mode = std::max(mode, 1_uc);
        poly90_set.emplace_back(std::move(data));
        return {&poly90_set, poly90_set.size() - 1};
    }

    inline PolyRef<Polygon45> add_poly45(point_vector_t &&data) {
        mode = std::max(mode, 2_uc);
        poly45_set.emplace_back(std::move(data));
        return {&poly45_set, poly45_set.size() - 1};
    }

    inline PolyRef<Polygon> add_poly(point_vector_t &&data) {
        mode = std::max(mode, 3_uc);
        poly_set.emplace_back(std::move(data));
        return {&poly_set, poly_set.size() - 1};
    }

  private:
    UnionViewVariant make_union_view();

    RectSet rect_set;
    Polygon90Set poly90_set;
    Polygon45Set poly45_set;
    PolygonSet poly_set;
    uint8_t mode;
    UnionViewVariant view;
};

using geo_map_t = std::unordered_map<layer_t, Geometry, boost::hash<layer_t>>;

class Via {
  public:
    inline Via(std::string via_id, uint32_t num_row, uint32_t num_col, uint32_t cut_w,
               uint32_t cut_h, int32_t cut_spx, int32_t cut_spy, int32_t lay1_encx,
               int32_t lay1_ency, int32_t lay1_offx, int32_t lay1_offy, int32_t lay2_encx,
               int32_t lay2_ency, int32_t lay2_offx, int32_t lay2_offy)
        : via_id(std::move(via_id)), num_row(num_row), num_col(num_col), cut_w(cut_w), cut_h(cut_h),
          cut_spacing(cut_spx, cut_spy), lay1_enc(lay1_encx, lay1_ency),
          lay1_off(lay1_offx, lay1_offy), lay2_enc(lay2_encx, lay2_ency),
          lay2_off(lay2_offx, lay2_offy), lay1_ref(), lay2_ref() {}

  private:
    std::string via_id;
    uint32_t num_row, num_col;
    uint32_t cut_w, cut_h;
    Vector cut_spacing;
    Vector lay1_enc;
    Vector lay1_off;
    Vector lay2_enc;
    Vector lay2_off;
    PolyRef<Rect> lay1_ref;
    PolyRef<Rect> lay2_ref;
};

class Blockage : public Polygon {
  public:
    explicit inline Blockage(point_vector_t data, BlockageType type)
        : Polygon(std::move(data)), type(type) {}

  private:
    BlockageType type;
};

using block_map_t = std::unordered_map<layer_t, std::vector<Blockage>, boost::hash<layer_t>>;

class Boundary : public Polygon {
  public:
    explicit inline Boundary(point_vector_t data, BoundaryType type)
        : Polygon(std::move(data)), type(type) {}

  private:
    BoundaryType type;
};

// class forwarding
class LayCellView;

class LayInstance {
  public:
    inline LayInstance(std::string lib, std::string cell, std::string view, Transform xform,
                       uint32_t nx = 1, uint32_t ny = 1, coord_t spx = 0, coord_t spy = 0)
        : lib(std::move(lib)), cell(std::move(cell)), view(std::move(view)), xform(xform), nx(nx),
          ny(ny), spx(spx), spy(spy), master(nullptr) {}

  private:
    std::string lib;
    std::string cell;
    std::string view;
    Transform xform;
    uint32_t nx;
    uint32_t ny;
    coord_t spx;
    coord_t spy;
    LayCellView *master;
};

using inst_map_t = std::unordered_map<std::string, LayInstance>;

class LayCellView {
  public:
    LayCellView(std::string tech, uint8_t geo_mode = 0)
        : tech(std::move(tech)), inst_name_cnt(0), geo_mode(geo_mode) {}

  private:
    std::string tech;
    geo_map_t geo_map;
    block_map_t lay_block_map;
    std::vector<Via> via_list;
    std::vector<Polygon> area_block_list;
    std::vector<Boundary> boundary_list;
    inst_map_t inst_list;
    uint32_t inst_name_cnt;
    uint8_t geo_mode;
};

} // namespace layout
} // namespace cbag

#endif // CBAG_LAYOUT_GEOMETRIES_H
