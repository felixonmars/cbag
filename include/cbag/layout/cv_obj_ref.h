#ifndef CBAG_LAYOUT_CV_OBJ_REF_H
#define CBAG_LAYOUT_CV_OBJ_REF_H

#include <vector>

#include <cbag/layout/cellview_fwd.h>
#include <cbag/layout/via.h>

namespace cbag {
namespace layout {

template <typename T> class shape_ref {
  private:
    layer_t key{0, 0};
    bool is_horiz = true;

  public:
    cellview *parent = nullptr;
    T obj;

    shape_ref() = default;

    shape_ref(const layer_t &key, T *ptr) : key(key), obj(std::move(*ptr)) {}

    shape_ref(cellview *parent, layer_t &&key, bool is_horiz, T &&obj, bool add)
        : key(std::move(key)), is_horiz(is_horiz), parent(parent), obj(std::move(obj)) {
        if (add)
            commit();
    }

    void commit() {
        if (parent != nullptr) {
            parent->get_geometry(key).add_shape(obj, is_horiz);
            parent = nullptr;
        }
    }
};

template <typename T> class cv_obj_ref {
  public:
    cellview *parent = nullptr;
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
