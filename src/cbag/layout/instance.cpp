#include <utility>

#include <cbag/layout/instance.h>

namespace cbag {
namespace layout {

cellview_ref::cellview_ref(const char *lib, const char *cell, const char *view)
    : lib(lib), cell(cell), view(view) {}

instance::instance() = default;

instance::instance(const char *lib, const char *cell, const char *view, transformation xform,
                   uint32_t nx, uint32_t ny, coord_t spx, coord_t spy)
    : xform(std::move(xform)), nx(nx), ny(ny), spx(spx), spy(spy),
      master(std::in_place_type_t<cellview_ref>{}, lib, cell, view) {}

instance::instance(const cellview *master, transformation xform, uint32_t nx, uint32_t ny,
                   coord_t spx, coord_t spy)
    : xform(std::move(xform)), nx(nx), ny(ny), spx(spx), spy(spy),
      master(std::in_place_type_t<const cellview *>{}, master) {}

void instance::set_int_param(const char *name, int value) {
    auto *cv_ref = std::get_if<cellview_ref>(&master);
    if (cv_ref != nullptr) {
        cv_ref->params[name] = value;
    }
}

void instance::set_double_param(const char *name, double value) {
    auto *cv_ref = std::get_if<cellview_ref>(&master);
    if (cv_ref != nullptr) {
        cv_ref->params[name] = value;
    }
}

void instance::set_bool_param(const char *name, bool value) {
    auto *cv_ref = std::get_if<cellview_ref>(&master);
    if (cv_ref != nullptr) {
        cv_ref->params[name] = value;
    }
}

void instance::set_string_param(const char *name, const char *value) {
    auto *cv_ref = std::get_if<cellview_ref>(&master);
    if (cv_ref != nullptr) {
        cv_ref->params[name] = std::string(value);
    }
}

} // namespace layout
} // namespace cbag
