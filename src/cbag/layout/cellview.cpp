#include <cstring>

#include <fmt/core.h>

#include <cbag/util/binary_iterator.h>

#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation_util.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/cv_obj_ref.h>
#include <cbag/layout/geo_iterator.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/via.h>

namespace cbag {
namespace layout {

struct cellview::helper {
    static void add_inst(cellview &self, const instance &inst) {
        if (!inst.name.empty()) {
            // test if given name is valid
            if (self.inst_map.emplace(std::string(inst.name), inst).second)
                return;
        }
        auto map_end = self.inst_map.end();
        cbag::util::binary_iterator<cnt_t> iter(self.inst_name_cnt);
        while (iter.has_next()) {
            if (self.inst_map.find(fmt::format("X{:d}", *iter)) == map_end) {
                iter.save();
                iter.down();
            } else {
                iter.up();
            }
        }

        self.inst_name_cnt = *(iter.get_save());
        self.inst_map.emplace(fmt::format("X{:d}", self.inst_name_cnt), inst);
    }
};

cellview::cellview(tech *tech_ptr, std::string cell_name, geometry_mode geo_mode)
    : geo_mode(geo_mode), tech_ptr(tech_ptr), cell_name(std::move(cell_name)) {}

void cellview::set_geometry_mode(geometry_mode new_mode) {
    if (!empty())
        throw std::runtime_error("Cannot change geometry mode of non-empty layout.");
    geo_mode = new_mode;
}

auto cellview::find_geometry(const layer_t &key) const -> decltype(geo_map.find(key)) {
    return geo_map.find(key);
}

geometry &cellview::make_geometry(const layer_t &key) {
    auto iter = geo_map.find(key);
    if (iter == geo_map.end()) {
        iter =
            geo_map.emplace(key, geometry(tech_ptr->get_layer_type(key.first), tech_ptr, geo_mode))
                .first;
    }
    return iter->second;
}

std::string cellview::get_name() const noexcept { return cell_name; }
tech *cellview::get_tech() const noexcept { return tech_ptr; }

bool cellview::empty() const noexcept {
    return geo_map.empty() && inst_map.empty() && via_list.empty() && lay_block_map.empty() &&
           area_block_list.empty() && boundary_list.empty() && pin_map.empty();
}

auto cellview::begin_inst() const -> decltype(inst_map.cbegin()) { return inst_map.cbegin(); }
auto cellview::end_inst() const -> decltype(inst_map.cend()) { return inst_map.cend(); }
auto cellview::begin_geometry() const -> decltype(geo_map.cbegin()) { return geo_map.cbegin(); }
auto cellview::end_geometry() const -> decltype(geo_map.cend()) { return geo_map.cend(); }
auto cellview::begin_via() const -> decltype(via_list.cbegin()) { return via_list.cbegin(); }
auto cellview::end_via() const -> decltype(via_list.cend()) { return via_list.cend(); }
auto cellview::begin_lay_block() const -> decltype(lay_block_map.cbegin()) {
    return lay_block_map.cbegin();
}
auto cellview::end_lay_block() const -> decltype(lay_block_map.cend()) {
    return lay_block_map.cend();
}
auto cellview::begin_area_block() const -> decltype(area_block_list.cbegin()) {
    return area_block_list.cbegin();
}
auto cellview::end_area_block() const -> decltype(area_block_list.cend()) {
    return area_block_list.cend();
}
auto cellview::begin_boundary() const -> decltype(boundary_list.cbegin()) {
    return boundary_list.cbegin();
}
auto cellview::end_boundary() const -> decltype(boundary_list.cend()) {
    return boundary_list.cend();
}
auto cellview::begin_pin() const -> decltype(pin_map.cbegin()) { return pin_map.cbegin(); }
auto cellview::end_pin() const -> decltype(pin_map.cend()) { return pin_map.cend(); }

void cellview::add_pin(const std::string &layer, std::string net, std::string label, box_t bbox) {
    auto lay_id = tech_ptr->get_layer_id(layer);
    auto iter = pin_map.find(lay_id);
    if (iter == pin_map.end()) {
        iter = pin_map.emplace(lay_id, std::vector<pin>()).first;
    }
    iter->second.emplace_back(std::move(bbox), std::move(net), std::move(label));
}

shape_ref<polygon90> cellview::add_poly90(const std::string &layer, const std::string &purpose,
                                          bool is_horiz, polygon90 &&poly, bool commit) {
    auto key = get_layer_t(*tech_ptr, layer, purpose);
    make_geometry(key);
    return {this, std::move(key), is_horiz, std::move(poly), commit};
}

shape_ref<polygon45> cellview::add_poly45(const std::string &layer, const std::string &purpose,
                                          bool is_horiz, polygon45 &&poly, bool commit) {
    auto key = get_layer_t(*tech_ptr, layer, purpose);
    make_geometry(key);
    return {this, std::move(key), is_horiz, std::move(poly), commit};
}

shape_ref<polygon45_set> cellview::add_poly45_set(const std::string &layer,
                                                  const std::string &purpose, bool is_horiz,
                                                  polygon45_set &&poly, bool commit) {
    auto key = get_layer_t(*tech_ptr, layer, purpose);
    make_geometry(key);
    return {this, std::move(key), is_horiz, std::move(poly), commit};
}

shape_ref<polygon> cellview::add_poly(const std::string &layer, const std::string &purpose,
                                      bool is_horiz, polygon &&poly, bool commit) {
    auto key = get_layer_t(*tech_ptr, layer, purpose);
    make_geometry(key);
    return {this, std::move(key), is_horiz, std::move(poly), commit};
}

cv_obj_ref<blockage> cellview::add_blockage(blockage &&data, bool commit) {
    return {this, std::move(data), commit};
}

cv_obj_ref<boundary> cellview::add_boundary(boundary &&data, bool commit) {
    return {this, std::move(data), commit};
}

cv_obj_ref<via> cellview::add_via(transformation xform, std::string via_id, bool add_layers,
                                  bool bot_horiz, bool top_horiz, cnt_t vnx, cnt_t vny, dist_t w,
                                  dist_t h, offset_t vspx, offset_t vspy, offset_t enc1l,
                                  offset_t enc1r, offset_t enc1t, offset_t enc1b, offset_t enc2l,
                                  offset_t enc2r, offset_t enc2t, offset_t enc2b, bool commit) {
    return {this,
            via(std::move(xform), std::move(via_id),
                via_param(vnx, vny, w, h, vspx, vspy, enc1l, enc1r, enc1t, enc1b, enc2l, enc2r,
                          enc2t, enc2b),
                add_layers, bot_horiz, top_horiz),
            commit};
}

void cellview::add_via_arr(const transformation &xform, const std::string &via_id, bool add_layers,
                           bool bot_horiz, bool top_horiz, cnt_t vnx, cnt_t vny, dist_t w, dist_t h,
                           offset_t vspx, offset_t vspy, offset_t enc1l, offset_t enc1r,
                           offset_t enc1t, offset_t enc1b, offset_t enc2l, offset_t enc2r,
                           offset_t enc2t, offset_t enc2b, cnt_t nx, cnt_t ny, offset_t spx,
                           offset_t spy) {
    via_param param{vnx,   vny,   w,     h,     vspx,  vspy,  enc1l,
                    enc1r, enc1t, enc1b, enc2l, enc2r, enc2t, enc2b};

    offset_t dx = 0;
    for (decltype(nx) xidx = 0; xidx < nx; ++xidx, dx += spx) {
        offset_t dy = 0;
        for (decltype(ny) yidx = 0; yidx < ny; ++yidx, dy += spy) {
            add_object(
                via(get_move_by(xform, dx, dy), via_id, param, add_layers, bot_horiz, top_horiz));
        }
    }
}

cv_obj_ref<instance> cellview::add_prim_instance(std::string lib, std::string cell,
                                                 std::string view, std::string name,
                                                 cbag::transformation xform, cnt_t nx, cnt_t ny,
                                                 offset_t spx, offset_t spy, bool commit) {
    return {this,
            instance(std::move(name), std::move(lib), std::move(cell), std::move(view),
                     std::move(xform), nx, ny, spx, spy),
            commit};
}

cv_obj_ref<instance> cellview::add_instance(const cellview *cv, std::string name,
                                            cbag::transformation xform, cnt_t nx, cnt_t ny,
                                            offset_t spx, offset_t spy, bool commit) {
    return {this, instance(std::move(name), cv, std::move(xform), nx, ny, spx, spy), commit};
}

void cellview::add_object(const blockage &obj) {
    if (obj.get_type() == blockage_type::placement) {
        // area blockage
        area_block_list.push_back(obj);
    } else {
        auto layer = obj.get_layer();
        auto iter = lay_block_map.find(layer);
        if (iter == lay_block_map.end()) {
            iter = lay_block_map.emplace(layer, std::vector<blockage>()).first;
        }
        iter->second.push_back(obj);
    }
}

void cellview::add_object(const boundary &obj) { boundary_list.push_back(obj); }

void cellview::add_object(const via &obj) {
    via_list.push_back(obj);
    if (obj.add_layers) {
        auto purpose = tech_ptr->get_purpose_id(nullptr);
        lay_t bot_lay, top_lay;
        tech_ptr->get_via_layers(obj.via_id, bot_lay, top_lay);
        layer_t bot_key(bot_lay, purpose);
        layer_t top_key(top_lay, purpose);
        auto &bot_geo = make_geometry(bot_key);
        auto &top_geo = make_geometry(top_key);
        bot_geo.add_shape(obj.bot_box(), obj.bot_horiz);
        top_geo.add_shape(obj.top_box(), obj.top_horiz);
    }
}

void cellview::add_object(const instance &obj) {
    helper::add_inst(*this, obj);
    auto master = obj.get_cellview();
    if (master != nullptr) {
        for (const auto &pair : master->geo_map) {
            auto &geo = make_geometry(pair.first);
            for (decltype(obj.nx) ix = 0; ix < obj.nx; ++ix) {
                for (decltype(obj.ny) iy = 0; iy < obj.ny; ++iy) {
                    // unordered map does not invalidate pointers to elements
                    geo.record_instance(&pair.second,
                                        get_move_by(obj.xform, obj.spx * ix, obj.spy * iy));
                }
            }
        }
    }
}

} // namespace layout
} // namespace cbag
