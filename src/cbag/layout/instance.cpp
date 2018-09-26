#include <utility>

#include <cbag/layout/cellview.h>
#include <cbag/layout/instance.h>

namespace cbag {
namespace layout {

class lib_name_visitor {
  private:
    const char *output_lib;

  public:
    explicit lib_name_visitor(const char *output_lib) : output_lib(output_lib) {}

    const char *operator()(const cellview *v) { return output_lib; }

    const char *operator()(const cellview_ref &v) { return v.lib.c_str(); }
};

class cell_name_visitor {
  private:
    const str_map_t *rename_map;

  public:
    explicit cell_name_visitor(const str_map_t *rename_map) : rename_map(rename_map) {}

    const char *operator()(const cellview *v) {
        if (rename_map == nullptr) {
            return v->cell_name.c_str();
        }
        return (*rename_map).find(v->cell_name)->second.c_str();
    }

    const char *operator()(const cellview_ref &v) { return v.cell.c_str(); }
};

class view_name_visitor {
  private:
    const char *view;

  public:
    explicit view_name_visitor(const char *view) : view(view) {}

    const char *operator()(const cellview *v) { return view; }

    const char *operator()(const cellview_ref &v) { return v.view.c_str(); }
};

class params_visitor {
  public:
    params_visitor() {}

    const param_map *operator()(const cellview *v) { return nullptr; }

    const param_map *operator()(const cellview_ref &v) { return &(v.params); }
};

cellview_ref::cellview_ref(const char *lib, const char *cell, const char *view)
    : lib(lib), cell(cell), view(view) {}

instance::instance() = default;

instance::instance(std::string name, const char *lib, const char *cell, const char *view,
                   transformation xform, uint32_t nx, uint32_t ny, coord_t spx, coord_t spy)
    : master(std::in_place_type_t<cellview_ref>{}, lib, cell, view), name(std::move(name)),
      xform(std::move(xform)), nx(nx), ny(ny), spx(spx), spy(spy) {}

instance::instance(std::string name, const cellview *master, transformation xform, uint32_t nx,
                   uint32_t ny, coord_t spx, coord_t spy)
    : master(std::in_place_type_t<const cellview *>{}, master), name(std::move(name)),
      xform(std::move(xform)), nx(nx), ny(ny), spx(spx), spy(spy) {}

const char *instance::get_lib_name(const char *output_lib) const {
    return std::visit(lib_name_visitor(output_lib), master);
}

const char *instance::get_cell_name(const str_map_t *rename_map) const {
    return std::visit(cell_name_visitor(rename_map), master);
}

const char *instance::get_view_name(const char *default_view) const {
    return std::visit(view_name_visitor(default_view), master);
}

const param_map *instance::get_params() const { return std::visit(params_visitor(), master); }

const cbag::transform instance::get_transform() const { return xform.to_transform(); }

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
