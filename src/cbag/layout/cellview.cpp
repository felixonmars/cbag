#include <tuple>

#include <cbag/util/binary_iterator.h>

#include <cbag/common/transformation_util.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/via_util.h>
#include <cbag/layout/via_wrapper.h>

namespace cbag {
namespace layout {

struct cellview::helper {
    static void add_inst(cellview &self, const instance &inst) {
        auto &inst_name = inst.get_inst_name();
        if (!inst_name.empty()) {
            // test if given name is valid
            if (self.inst_map.emplace(inst_name, inst).second)
                return;
        }
        auto map_end = self.inst_map.end();
        cbag::util::binary_iterator<cnt_t> iter(self.inst_name_cnt);
        while (iter.has_next()) {
            if (self.inst_map.find("X" + std::to_string(*iter)) == map_end) {
                iter.save();
                iter.down();
            } else {
                iter.up();
            }
        }

        self.inst_name_cnt = *(iter.get_save());
        self.inst_map.emplace("X" + std::to_string(self.inst_name_cnt), inst);
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

const std::string &cellview::get_name() const noexcept { return cell_name; }
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
    auto lay_id = layer_id_at(*tech_ptr, layer);
    auto iter = pin_map.find(lay_id);
    if (iter == pin_map.end()) {
        iter = pin_map.emplace(lay_id, std::vector<pin>()).first;
    }
    iter->second.emplace_back(std::move(bbox), std::move(net), std::move(label));
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

void cellview::add_object(const via_wrapper &obj) {
    via_list.push_back(obj.v);
    if (obj.add_layers) {
        auto [bot_key, unused, top_key] = tech_ptr->get_via_layer_purpose(obj.v.get_via_id());
        (void)unused;
        make_geometry(bot_key).add_shape(get_bot_box(obj.v), obj.bot_horiz);
        make_geometry(top_key).add_shape(get_top_box(obj.v), obj.top_horiz);
    }
}

void cellview::add_object(const instance &obj) {
    helper::add_inst(*this, obj);
    auto master = obj.get_cellview();
    if (master != nullptr) {
        for (const auto &[layer_key, inst_geo] : master->geo_map) {
            auto &geo = make_geometry(layer_key);
            for (decltype(obj.nx) ix = 0; ix < obj.nx; ++ix) {
                for (decltype(obj.ny) iy = 0; iy < obj.ny; ++iy) {
                    // unordered map does not invalidate pointers to elements
                    geo.record_instance(&inst_geo,
                                        get_move_by(obj.xform, obj.spx * ix, obj.spy * iy));
                }
            }
        }
    }
}

} // namespace layout
} // namespace cbag
