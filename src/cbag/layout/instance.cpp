#include <utility>

#include <cbag/layout/cellview.h>
#include <cbag/layout/instance.h>
#include <cbag/layout/rectangle.h>
#include <cbag/util/overload.h>

namespace cbag {
namespace layout {

cellview_ref::cellview_ref(const char *lib, const char *cell, const char *view)
    : lib(lib), cell(cell), view(view) {}

instance::instance() = default;

instance::instance(const char *name, const char *lib, const char *cell, const char *view,
                   transformation xform, uint32_t nx, uint32_t ny, coord_t spx, coord_t spy)
    : master(std::in_place_type_t<cellview_ref>{}, lib, cell, view), name(name),
      xform(std::move(xform)), nx(nx), ny(ny), spx(spx), spy(spy) {}

instance::instance(const char *name, const cellview *master, transformation xform, uint32_t nx,
                   uint32_t ny, coord_t spx, coord_t spy)
    : master(std::in_place_type_t<const cellview *>{}, master), name(name), xform(std::move(xform)),
      nx(nx), ny(ny), spx(spx), spy(spy) {}

const char *instance::get_lib_name(const char *output_lib) const {
    return std::visit(
        overload{
            [&](const cellview *v) { return output_lib; },
            [&](const cellview_ref &v) { return v.lib.c_str(); },
        },
        master);
}

const char *instance::get_cell_name(const str_map_t *rename_map) const {
    return std::visit(
        overload{
            [&](const cellview *v) {
                if (rename_map == nullptr) {
                    return v->cell_name.c_str();
                }
                return (*rename_map).find(v->cell_name)->second.c_str();
            },
            [&](const cellview_ref &v) { return v.cell.c_str(); },
        },
        master);
}

const char *instance::get_view_name(const char *default_view) const {
    return std::visit(
        overload{
            [&](const cellview *v) { return default_view; },
            [&](const cellview_ref &v) { return v.view.c_str(); },
        },
        master);
}

const param_map *instance::get_params() const {
    return std::visit(
        overload{
            [&](const cellview *v) { return (const param_map *)nullptr; },
            [&](const cellview_ref &v) { return &(v.params); },
        },
        master);
}

const cbag::transform instance::get_transform() const { return xform.to_transform(); }

rectangle instance::get_bbox(const char *layer, const char *purpose) const {
    rectangle r = std::visit(
        overload{
            [&](const cellview *v) { return v->get_bbox(layer, purpose); },
            [&](const cellview_ref &v) { return rectangle(0, 0, 0, 0); },
        },
        master);

    return r.transform(xform);
}

void instance::set_master(const cellview *new_master) { master = new_master; }

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
