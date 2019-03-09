#ifndef CBAG_OA_OA_POLYGON_H
#define CBAG_OA_OA_POLYGON_H

#include <boost/polygon/polygon.hpp>

#include <oa/oaDesignDB.h>

namespace bp = boost::polygon;

namespace spdlog {
class logger;
}

namespace OpenAccess_4 {
template <class T> class ByteAppDef : public oaIntAppDef<T> {
  public:
    static ByteAppDef<T> *get(const oaString &name, oaByte defValue = 0, oaBoolean persist = true) {
        return (ByteAppDef<T> *)oaIntAppDef<T>::get(name, defValue - 1, persist);
    }

    static ByteAppDef<T> *get(const oaString &name, const oaAppObjectDef *objDef,
                              oaByte defValue = 0, oaBoolean persist = true) {
        return (ByteAppDef<T> *)oaIntAppDef<T>::get(name, objDef, defValue - 1, persist);
    }

    static ByteAppDef<T> *find(const oaString &name) {
        return (ByteAppDef<T> *)oaIntAppDef<T>::find(name);
    }

    static ByteAppDef<T> *find(const oaString &name, const oaAppObjectDef *objDef) {
        return (ByteAppDef<T> *)oaIntAppDef<T>::find(name, objDef);
    }

    oaByte get(const T *object) { return oaIntAppDef<T>::get(object) + 1; }

    void set(T *object, oaByte value) { oaIntAppDef<T>::set(object, (oaInt4)value - 1); }

    oaByte getDefault() const { return (oaByte)oaIntAppDef<T>::getDefault() + 1; }
};
} // namespace OpenAccess_4

namespace cbagoa {

class oa_polygon {
  public:
    oa::oaPointArray pt_arr;

    oa_polygon() = default;

    const oa::oaPoint *begin() const { return pt_arr.getElements(); }
    const oa::oaPoint *end() const { return pt_arr.getElements() + pt_arr.getNumElements(); }
};

class polygon_writer {
  public:
    using value_type = oa_polygon;

  private:
    oa::oaBlock *block;
    oa::oaLayerNum layer;
    oa::oaPurposeNum purpose;
    spdlog::logger &logger;
    oa::ByteAppDef<oa::oaShape> *color_def_ptr;
    oa::oaByte color;
    value_type last;

  public:
    polygon_writer(oa::oaBlock *blk, oa::oaLayerNum lay, oa::oaPurposeNum purp,
                   spdlog::logger &logger, oa::ByteAppDef<oa::oaShape> *color_def_ptr,
                   oa::oaByte color)
        : block(blk), layer(lay), purpose(purp), logger(logger), color_def_ptr(color_def_ptr),
          color(color) {}

    void push_back(value_type &&v) {
        record_last();
        last = std::move(v);
    }

    void insert(value_type *ptr, const value_type &v) {
        record_last();
        last = v;
    }

    void record_last() const {
        if (last.pt_arr.getNumElements() > 0) {
            oa::oaShape *ptr;
            if (last.pt_arr.isRectangle()) {
                oa::oaBox box;
                last.pt_arr.getBBox(box);
                ptr = oa::oaRect::create(block, layer, purpose, box);
            } else {
                ptr = oa::oaPolygon::create(block, layer, purpose, last.pt_arr);
            }
            if (color_def_ptr) {
                color_def_ptr->set(ptr, color);
            }
        }
    }

    value_type &back() { return last; }

    value_type *end() const { return nullptr; }
};

} // namespace cbagoa

namespace boost {
namespace polygon {
template <> struct geometry_concept<oa::oaPoint> { typedef point_concept type; };
template <> struct point_traits<oa::oaPoint> {
    using coordinate_type = oa::oaCoord;

    static inline coordinate_type get(const oa::oaPoint &point, orientation_2d orient) {
        return (orient == HORIZONTAL) ? point.x() : point.y();
    }
};

template <> struct point_mutable_traits<oa::oaPoint> {
    using coordinate_type = oa::oaCoord;

    static inline void set(oa::oaPoint &point, orientation_2d orient, coordinate_type value) {
        if (orient == HORIZONTAL)
            point.x() = value;
        else
            point.y() = value;
    }
    static inline oa::oaPoint construct(coordinate_type x_value, coordinate_type y_value) {
        return oa::oaPoint{x_value, y_value};
    }
};

template <> struct geometry_concept<cbagoa::oa_polygon> { typedef polygon_concept type; };

template <> struct polygon_traits<cbagoa::oa_polygon> {
    using coordinate_type = oa::oaCoord;
    using point_type = oa::oaPoint;
    using iterator_type = const point_type *;

    // Get the begin iterator
    static inline iterator_type begin_points(const cbagoa::oa_polygon &t) { return t.begin(); }

    // Get the end iterator
    static inline iterator_type end_points(const cbagoa::oa_polygon &t) { return t.end(); }

    // Get the number of sides of the polygon
    static inline std::size_t size(const cbagoa::oa_polygon &t) {
        return t.pt_arr.getNumElements();
    }

    // Get the winding direction of the polygon
    static inline winding_direction winding(const cbagoa::oa_polygon &t) { return unknown_winding; }
};

template <> struct polygon_mutable_traits<cbagoa::oa_polygon> {
    // expects stl style iterators
    template <typename iT>
    static inline cbagoa::oa_polygon &set_points(cbagoa::oa_polygon &t, iT input_begin,
                                                 iT input_end) {
        t.pt_arr.setNumElements(0);
        for (; input_begin != input_end; ++input_begin) {
            t.pt_arr.append(oa::oaPoint(bp::x(*input_begin), bp::y(*input_begin)));
        }
        // remove last point if same as first point
        auto n = t.pt_arr.getNumElements();
        if (t.pt_arr[n - 1] == t.pt_arr[0])
            t.pt_arr.setNumElements(n - 1);
        return t;
    }
};
} // namespace polygon
} // namespace boost

#endif
