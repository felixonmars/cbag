#ifndef CBAG_LAYOUT_CV_OBJ_REF_H
#define CBAG_LAYOUT_CV_OBJ_REF_H

#include <vector>

#include <cbag/layout/cellview.h>
#include <cbag/layout/typedefs.h>
#include <cbag/layout/via.h>

namespace cbag {
namespace layout {

template <typename T> class shape_ref {
  private:
    cellview *parent = nullptr;
    layer_t key{0, 0};

  public:
    T obj;

    shape_ref() = default;

    shape_ref(cellview *parent, layer_t &&key, T &&obj, bool add)
        : parent(parent), key(std::move(key)), obj(std::move(obj)) {
        if (add)
            commit();
    }

    void commit() {
        if (parent != nullptr) {
            parent->geo_map[key].add_shape(obj);
            parent = nullptr;
        }
    }
};

template <typename T> class cv_obj_ref {
  private:
    cellview *parent = nullptr;

  public:
    T obj;

    cv_obj_ref() = default;

    cv_obj_ref(cellview *parent, T &&obj, bool add) : parent(parent), obj(std::move(obj)) {
        if (add)
            commit();
    }

    void commit() {
        if (parent != nullptr) {
            parent->add_object(obj);
            parent = nullptr;
        }
    }
};

} // namespace layout
} // namespace cbag

#endif
