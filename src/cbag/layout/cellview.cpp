#include <cstring>

#include <fmt/format.h>

#include <cbag/util/binary_iterator.h>

#include <cbag/common/blockage_type.h>
#include <cbag/common/boundary_type.h>
#include <cbag/common/box_t.h>
#include <cbag/layout/blockage.h>
#include <cbag/layout/boundary.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/cv_obj_ref.h>
#include <cbag/layout/geo_iterator.h>
#include <cbag/layout/pin.h>
#include <cbag/layout/tech.h>
#include <cbag/layout/via.h>

namespace cbag {
namespace layout {

struct cellview::helper {
    static void add_inst(cellview &self, const instance &inst) {
        if (inst.name != nullptr) {
            // test if given name is valid
            if (self.inst_map.emplace(std::string(inst.name), inst).second)
                return;
        }
        auto map_end = self.inst_map.end();
        cbag::util::binary_iterator<uint32_t> iter(self.inst_name_cnt);
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

    static geo_map_t::iterator make_geometry(cellview &self, const layer_t &key) {
        auto iter = self.geo_map.find(key);
        if (iter == self.geo_map.end()) {
            iter = self.geo_map
                       .emplace(key, geometry(self.tech_ptr->get_layer_type(key.first),
                                              self.tech_ptr, self.geo_mode))
                       .first;
        }
        return iter;
    }
};

cellview::cellview(tech *tech_ptr, const char *cell_name, uint8_t geo_mode)
    : geo_mode(geo_mode), tech_ptr(tech_ptr), cell_name(cell_name) {}

void cellview::set_geometry_mode(uint8_t new_mode) {
    if (!empty())
        throw std::runtime_error("Cannot change geometry mode of non-empty layout.");
    geo_mode = new_mode;
}

bool cellview::empty() const {
    return geo_map.empty() && inst_map.empty() && via_list.empty() && lay_block_map.empty() &&
           area_block_list.empty() && boundary_list.empty() && pin_map.empty();
}

layer_t cellview::get_lay_purp_key(const char *layer, const char *purpose) const {
    lay_t lay_id = tech_ptr->get_layer_id(layer);
    purp_t purp_id = tech_ptr->get_purpose_id(purpose);
    return {lay_id, purp_id};
}

box_t cellview::get_bbox(const char *layer, const char *purpose) const {
    box_t ans(0, 0, -1, -1);
    // merge geometry bounding box
    auto iter = geo_map.find(get_lay_purp_key(layer, purpose));
    if (iter != geo_map.end()) {
        ans.merge(iter->second.get_bbox());
    }
    // merge instance bounding box
    for (const auto &p : inst_map) {
        ans.merge(p.second.get_bbox(layer, purpose));
    }
    return ans;
}

geo_iterator cellview::begin_intersect(const layer_t &key, const box_t &r, offset_t spx,
                                       offset_t spy) const {
    auto iter = geo_map.find(key);
    if (iter == geo_map.end())
        return {};
    return iter->second.begin_intersect(r, spx, spy);
}

geo_iterator cellview::end_intersect() const { return {}; }

void cellview::add_pin(const char *layer, coord_t xl, coord_t yl, coord_t xh, coord_t yh,
                       const char *net, const char *label) {
    lay_t lay_id = tech_ptr->get_layer_id(layer);
    auto iter = pin_map.find(lay_id);
    if (iter == pin_map.end()) {
        iter = pin_map.emplace(lay_id, std::vector<pin>()).first;
    }
    iter->second.emplace_back(xl, yl, xh, yh, net, label);
}

shape_ref<box_t> cellview::add_rect(const char *layer, const char *purpose, bool is_horiz,
                                    coord_t xl, coord_t yl, coord_t xh, coord_t yh, bool commit) {
    layer_t key = get_lay_purp_key(layer, purpose);
    helper::make_geometry(*this, key);
    return {this, std::move(key), is_horiz, box_t(xl, yl, xh, yh), commit};
}

shape_ref<polygon90> cellview::add_poly90(const char *layer, const char *purpose, bool is_horiz,
                                          const pt_vector &data, bool commit) {
    layer_t key = get_lay_purp_key(layer, purpose);
    helper::make_geometry(*this, key);
    polygon90 val;
    val.set(data.begin(), data.end());
    return {this, std::move(key), is_horiz, std::move(val), commit};
}

shape_ref<polygon45> cellview::add_poly45(const char *layer, const char *purpose, bool is_horiz,
                                          const pt_vector &data, bool commit) {
    layer_t key = get_lay_purp_key(layer, purpose);
    helper::make_geometry(*this, key);
    polygon45 val;
    val.set(data.begin(), data.end());
    return {this, std::move(key), is_horiz, std::move(val), commit};
}

shape_ref<polygon> cellview::add_poly(const char *layer, const char *purpose, bool is_horiz,
                                      const pt_vector &data, bool commit) {
    layer_t key = get_lay_purp_key(layer, purpose);
    helper::make_geometry(*this, key);
    polygon val;
    val.set(data.begin(), data.end());
    return {this, std::move(key), is_horiz, std::move(val), commit};
}

shape_ref<polygon45_set> cellview::add_path(const char *layer, const char *purpose, bool is_horiz,
                                            const pt_vector &data, offset_t half_width,
                                            uint8_t style0, uint8_t style1, uint8_t stylem,
                                            bool commit) {
    layer_t key = get_lay_purp_key(layer, purpose);
    helper::make_geometry(*this, key);
    return {this, std::move(key), is_horiz,
            geometry::make_path(data, half_width, style0, style1, stylem), commit};
}

shape_ref<polygon45_set> cellview::add_path45_bus(const char *layer, const char *purpose,
                                                  bool is_horiz, const pt_vector &data,
                                                  const std::vector<offset_t> &widths,
                                                  const std::vector<offset_t> &spaces,
                                                  uint8_t style0, uint8_t style1, uint8_t stylem,
                                                  bool commit) {
    layer_t key = get_lay_purp_key(layer, purpose);
    helper::make_geometry(*this, key);
    return {this, std::move(key), is_horiz,
            geometry::make_path45_bus(data, widths, spaces, style0, style1, stylem), commit};
}

cv_obj_ref<blockage> cellview::add_blockage(const char *layer, uint8_t blk_code,
                                            const pt_vector &data, bool commit) {
    auto btype = static_cast<blockage_type>(blk_code);
    lay_t lay_id = (layer == nullptr || strlen(layer) == 0) ? 0 : tech_ptr->get_layer_id(layer);
    return {this, blockage(data, btype, lay_id), commit};
}

cv_obj_ref<boundary> cellview::add_boundary(uint8_t bnd_code, const pt_vector &data, bool commit) {
    auto btype = static_cast<boundary_type>(bnd_code);
    return {this, boundary(data, btype), commit};
}

cv_obj_ref<via> cellview::add_via(cbag::transformation xform, const char *via_id,
                                  const uint32_t (&num)[2], const dist_t (&cut_dim)[2],
                                  const offset_t (&cut_sp)[2], const offset_t (&lay1_enc)[2],
                                  const offset_t (&lay1_off)[2], const offset_t (&lay2_enc)[2],
                                  const offset_t (&lay2_off)[2], bool add_layers, bool bot_horiz,
                                  bool top_horiz, bool commit) {
    return {this,
            via(xform, via_id, num, cut_dim, cut_sp, lay1_enc, lay1_off, lay2_enc, lay2_off,
                add_layers, bot_horiz, top_horiz),
            commit};
}

cv_obj_ref<instance> cellview::add_prim_instance(const char *lib, const char *cell,
                                                 const char *view, const char *name,
                                                 cbag::transformation xform, uint32_t nx,
                                                 uint32_t ny, offset_t spx, offset_t spy,
                                                 bool commit) {
    return {this, instance(name, lib, cell, view, std::move(xform), nx, ny, spx, spy), commit};
}

cv_obj_ref<instance> cellview::add_instance(const cellview *cv, const char *name,
                                            cbag::transformation xform, uint32_t nx, uint32_t ny,
                                            offset_t spx, offset_t spy, bool commit) {
    return {this, instance(name, cv, std::move(xform), nx, ny, spx, spy), commit};
}

void cellview::add_object(const blockage &obj) {
    if (obj.type == blkPlacement) {
        // area blockage
        area_block_list.push_back(obj);
    } else {
        auto iter = lay_block_map.find(obj.layer);
        if (iter == lay_block_map.end()) {
            iter = lay_block_map.emplace(obj.layer, std::vector<blockage>()).first;
        }
        iter->second.push_back(obj);
    }
}

void cellview::add_object(const boundary &obj) { boundary_list.push_back(obj); }

void cellview::add_object(const via &obj) {
    via_list.push_back(obj);
    if (obj.add_layers) {
        purp_t purpose = tech_ptr->get_purpose_id(nullptr);
        lay_t bot_lay, top_lay;
        tech_ptr->get_via_layers(obj.via_id, bot_lay, top_lay);
        layer_t bot_key(bot_lay, purpose);
        layer_t top_key(top_lay, purpose);
        auto bot_iter = helper::make_geometry(*this, bot_key);
        auto top_iter = helper::make_geometry(*this, top_key);
        bot_iter->second.add_shape(obj.bot_box(), obj.bot_horiz);
        top_iter->second.add_shape(obj.top_box(), obj.top_horiz);
    }
}

void cellview::add_object(const instance &obj) {
    helper::add_inst(*this, obj);
    const cellview *master = obj.get_cellview();
    if (master != nullptr) {
        for (const auto &pair : master->geo_map) {
            auto geo_iter = helper::make_geometry(*this, pair.first);
            for (uint32_t ix = 0; ix < obj.nx; ++ix) {
                for (uint32_t iy = 0; iy < obj.ny; ++iy) {
                    // unordered map does not invalidate pointers to elements
                    geo_iter->second.record_instance(
                        &pair.second, obj.xform.get_move_by(obj.spx * ix, obj.spy * iy));
                }
            }
        }
    }
}

} // namespace layout
} // namespace cbag
