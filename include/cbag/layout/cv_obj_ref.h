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
    cellview *parent = nullptr;
    T obj;

  public:
    shape_ref() = default;

    shape_ref(cellview *parent, layer_t &&key, T &&obj, bool add)
        : key(std::move(key)), parent(parent), obj(std::move(obj)) {
        if (add)
            commit();
    }

    T *operator->() const noexcept { return &obj; }

    bool editable() const noexcept { return parent != nullptr; }

    void commit() {
        if (editable()) {
            if constexpr (std::is_base_of_v<polygon45_set, std::decay_t<T>>) {
                parent->make_geometry(key).add_shape_set(*(parent->get_grid()), key, obj);
                parent = nullptr;
            } else {
                auto [spx, spy] = get_margins(*(parent->get_grid()), key, obj);
                parent->make_geometry(key).add_shape(obj, spx, spy);
                parent = nullptr;
            }
        }
    }
};

template <typename T> class cv_obj_ref {
  private:
    cellview *parent = nullptr;
    T obj;

  public:
    cv_obj_ref() = default;

    cv_obj_ref(cellview *parent, T &&obj, bool add) : parent(parent), obj(std::move(obj)) {
        if (add)
            commit();
    }

    T *operator->() const noexcept { return &obj; }

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
