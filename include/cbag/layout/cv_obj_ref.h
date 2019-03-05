#ifndef CBAG_LAYOUT_CV_OBJ_REF_H
#define CBAG_LAYOUT_CV_OBJ_REF_H

#include <vector>

#include <cbag/layout/cellview_fwd.h>
#include <cbag/layout/routing_grid_util.h>

namespace cbag {
namespace layout {

template <typename T> class shape_ref {
  private:
    layer_t key{0, 0};
    std::shared_ptr<cellview> parent = nullptr;
    T obj;

  public:
    shape_ref() = default;

    shape_ref(std::shared_ptr<cellview> parent, layer_t &&key, T &&obj, bool add)
        : key(std::move(key)), parent(std::move(parent)), obj(std::move(obj)) {
        if (add)
            commit();
    }

    const T *operator->() const noexcept { return &obj; }
    T *operator->() noexcept { return &obj; }

    bool editable() const noexcept { return parent != nullptr; }

    void commit() {
        if (editable()) {
            parent->add_shape(key, obj);
        }
    }
};

template <typename T> class cv_obj_ref {
  private:
    std::shared_ptr<cellview> parent = nullptr;
    T obj;

  public:
    cv_obj_ref() = default;

    cv_obj_ref(std::shared_ptr<cellview> parent, T &&obj, bool add)
        : parent(std::move(parent)), obj(std::move(obj)) {
        if (add)
            commit();
    }

    const T *operator->() const noexcept { return &obj; }
    T *operator->() noexcept { return &obj; }

    bool editable() const noexcept { return parent != nullptr; }

    void commit() {
        if (editable()) {
            parent->add_object(obj);
            parent = nullptr;
        }
    }
};

} // namespace layout
} // namespace cbag

#endif
