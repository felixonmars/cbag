#include <utility>

#include <cbag/common/box_t_util.h>
#include <cbag/common/param_map_util.h>
#include <cbag/layout/cellview_util.h>
#include <cbag/layout/instance.h>
#include <cbag/util/overload.h>

namespace cbag {
namespace layout {

cellview_ref::cellview_ref(std::string lib, std::string cell, std::string view)
    : lib(std::move(lib)), cell(std::move(cell)), view(std::move(view)) {}

bool cellview_ref::operator==(const cellview_ref &rhs) const noexcept {
    return lib == rhs.lib && cell == rhs.cell && view == rhs.view && params == rhs.params;
}

instance::instance() = default;

instance::instance(std::string name, std::string lib, std::string cell, std::string view,
                   cbag::transformation xform, cnt_t nx, cnt_t ny, coord_t spx, coord_t spy)
    : master(std::in_place_type_t<cellview_ref>{}, std::move(lib), std::move(cell),
             std::move(view)),
      name(std::move(name)), xform(std::move(xform)), nx(nx), ny(ny), spx(spx), spy(spy) {}

instance::instance(std::string name, std::shared_ptr<const cellview> master,
                   cbag::transformation xform, cnt_t nx, cnt_t ny, coord_t spx, coord_t spy)
    : master(std::in_place_type_t<std::shared_ptr<const cellview>>{}, std::move(master)),
      name(std::move(name)), xform(std::move(xform)), nx(nx), ny(ny), spx(spx), spy(spy) {}

bool master_equal(const std::variant<std::shared_ptr<const cellview>, cellview_ref> &lhs,
                  const std::variant<std::shared_ptr<const cellview>, cellview_ref> &rhs) {
    return std::visit(
        overload{
            [](const std::shared_ptr<const cellview> &a, const std::shared_ptr<const cellview> &b) {
                return *a == *b;
            },
            [](const cellview_ref &a, const cellview_ref &b) { return a == b; },
            [](auto, auto) { return false; },
        },
        lhs, rhs);
}

bool instance::operator==(const instance &rhs) const noexcept {
    return master_equal(master, rhs.master) && name == rhs.name && xform == rhs.xform &&
           nx == rhs.nx && ny == rhs.ny && spx == rhs.spx && spy == rhs.spy;
}

bool instance::is_reference() const { return std::holds_alternative<cellview_ref>(master); }

const cellview *instance::get_cellview() const {
    auto ptr = std::get_if<std::shared_ptr<const cellview>>(&master);
    return (ptr == nullptr) ? nullptr : (*ptr).get();
}

const std::string &instance::get_inst_name() const { return name; }

const std::string &instance::get_lib_name(const std::string &output_lib) const {
    return std::visit(
        overload{
            [&output_lib](const std::shared_ptr<const cellview> &v) -> const std::string & {
                return output_lib;
            },
            [](const cellview_ref &v) -> const std::string & { return v.lib; },
        },
        master);
}

const std::string &instance::get_cell_name(const str_map_t *rename_map) const {
    return std::visit(
        overload{
            [&rename_map](const std::shared_ptr<const cellview> &v) -> const std::string & {
                if (rename_map == nullptr) {
                    return v->get_name();
                }
                auto iter = rename_map->find(v->get_name());
                if (iter == rename_map->end())
                    return v->get_name();
                return iter->second;
            },
            [](const cellview_ref &v) -> const std::string & { return v.cell; },
        },
        master);
} // namespace layout

const std::string &instance::get_view_name(const std::string &default_view) const {
    return std::visit(
        overload{
            [&default_view](const std::shared_ptr<const cellview> &v) -> const std::string & {
                return default_view;
            },
            [](const cellview_ref &v) -> const std::string & { return v.view; },
        },
        master);
}

const param_map *instance::get_params() const {
    return std::visit(
        overload{
            [](const std::shared_ptr<const cellview> &v) { return (const param_map *)nullptr; },
            [](const cellview_ref &v) { return &(v.params); },
        },
        master);
}

box_t instance::get_bbox(const std::string &layer, const std::string &purpose) const {
    auto r = std::visit(
        overload{
            [&layer, &purpose](const std::shared_ptr<const cellview> &v) {
                return cbag::layout::get_bbox(*v, layer, purpose);
            },
            [](const cellview_ref &v) {
                return box_t{0, 0, 0, 0};
            },
        },
        master);

    return transform(r, xform);
}

void instance::set_master(const std::shared_ptr<const cellview> &new_master) {
    master = new_master;
}

void instance::set_param(const std::string &name, const param_t &val) {
    auto *cv_ref = std::get_if<cellview_ref>(&master);
    if (cv_ref != nullptr) {
        cbag::set_param(cv_ref->params, name, val);
    }
}

} // namespace layout
} // namespace cbag
