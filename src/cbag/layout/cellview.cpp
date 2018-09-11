
#include <fmt/format.h>

#include <cbag/util/binary_iterator.h>

#include <cbag/layout/blockage.h>
#include <cbag/layout/boundary.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/geometry.h>
#include <cbag/layout/polygon_ref.h>
#include <cbag/layout/rectangle.h>
#include <cbag/layout/tech.h>
#include <cbag/layout/via.h>
#include <cbag/layout/via_ref.h>

namespace cbag {
namespace layout {

struct cellview::helper {
    static std::string get_inst_name(cellview &self, const char *name) {
        auto map_end = self.inst_map.end();
        if (name != nullptr) {
            // test if given name is valid
            std::string test(name);
            if (self.inst_map.find(test) == map_end) {
                // valid name, just return it
                return test;
            }
        }
        cbag::util::binary_iterator<uint32_t> iter(self.inst_name_cnt,
                                                   std::make_optional<uint32_t>());
        while (iter.has_next()) {
            if (self.inst_map.find(fmt::format("X{d}", *iter)) == map_end) {
                iter.save();
                iter.down();
            } else {
                iter.up();
            }
        }

        self.inst_name_cnt = *(iter.get_save());
        return fmt::format("X{d}", self.inst_name_cnt);
    }

    static geo_map_t::iterator get_geometry(cellview &self, const layer_t &layer) {
        geo_map_t::iterator iter = self.geo_map.find(layer);
        if (iter == self.geo_map.end()) {
            iter = self.geo_map.emplace(layer, geometry(self.geo_mode)).first;
        }
        return iter;
    }
};

cellview::cellview(tech *tech_ptr, const char *cell_name, uint8_t geo_mode)
    : tech_ptr(tech_ptr), geo_mode(geo_mode), cell_name(cell_name) {}

rectangle cellview::get_bbox(const char *layer, const char *purpose) const {
    lay_t lay_id = tech_ptr->get_layer_id(layer);
    purp_t purp_id = tech_ptr->get_purpose_id(purpose);
    auto iter = geo_map.find(layer_t(lay_id, purp_id));
    if (iter == geo_map.end()) {
        return rectangle(0, 0, -1, -1);
    }
    return iter->second.get_bbox();
}

polygon_ref<rectangle> cellview::add_rect(const char *layer, const char *purpose, coord_t xl,
                                          coord_t yl, coord_t xh, coord_t yh) {
    lay_t lay_id = tech_ptr->get_layer_id(layer);
    purp_t purp_id = tech_ptr->get_purpose_id(purpose);
    return add_rect(lay_id, purp_id, xl, yl, xh, yh);
}

polygon_ref<rectangle> cellview::add_rect(lay_t lay_id, purp_t purp_id, coord_t xl, coord_t yl,
                                          coord_t xh, coord_t yh) {
    auto iter = helper::get_geometry(*this, layer_t(lay_id, purp_id));
    return iter->second.add_rect(xl, yl, xh, yh);
}

polygon_ref<polygon90> cellview::add_poly90(const char *layer, const char *purpose,
                                            pt_vector data) {
    lay_t lay_id = tech_ptr->get_layer_id(layer);
    purp_t purp_id = tech_ptr->get_purpose_id(purpose);
    layer_t key(lay_id, purp_id);
    auto iter = geo_map.find(key);
    if (iter == geo_map.end()) {
        iter = geo_map.emplace(std::move(key), geometry(geo_mode)).first;
    }
    return iter->second.add_poly90(std::move(data));
}

polygon_ref<polygon45> cellview::add_poly45(const char *layer, const char *purpose,
                                            pt_vector data) {
    lay_t lay_id = tech_ptr->get_layer_id(layer);
    purp_t purp_id = tech_ptr->get_purpose_id(purpose);
    layer_t key(lay_id, purp_id);
    auto iter = geo_map.find(key);
    if (iter == geo_map.end()) {
        iter = geo_map.emplace(std::move(key), geometry(geo_mode)).first;
    }
    return iter->second.add_poly45(std::move(data));
}

polygon_ref<polygon> cellview::add_poly(const char *layer, const char *purpose, pt_vector data) {
    lay_t lay_id = tech_ptr->get_layer_id(layer);
    purp_t purp_id = tech_ptr->get_purpose_id(purpose);
    layer_t key(lay_id, purp_id);
    auto iter = geo_map.find(key);
    if (iter == geo_map.end()) {
        iter = geo_map.emplace(std::move(key), geometry(geo_mode)).first;
    }
    return iter->second.add_poly(std::move(data));
}

void cellview::add_path_seg(const char *layer, const char *purpose, coord_t x0, coord_t y0,
                            coord_t x1, coord_t y1, dist_t width, const char *style0,
                            const char *style1) {}

inst_iter_t cellview::add_prim_instance(const char *lib, const char *cell, const char *view,
                                        const char *name, transformation xform, uint32_t nx,
                                        uint32_t ny, offset_t spx, offset_t spy) {
    std::string key = helper::get_inst_name(*this, name);

    return inst_map
        .emplace(helper::get_inst_name(*this, name),
                 instance(lib, cell, view, std::move(xform), nx, ny, spx, spy))
        .first;
}

inst_iter_t cellview::add_instance(const cellview *cv, const char *name, transformation xform,
                                   uint32_t nx, uint32_t ny, offset_t spx, offset_t spy) {
    std::string key = helper::get_inst_name(*this, name);

    return inst_map
        .emplace(helper::get_inst_name(*this, name),
                 instance(cv, std::move(xform), nx, ny, spx, spy))
        .first;
}

via_ref cellview::add_via(transformation xform, const char *via_id, const uint32_t (&num)[2],
                          const dist_t (&cut_dim)[2], const offset_t (&cut_sp)[2],
                          const offset_t (&lay1_enc)[2], const offset_t (&lay1_off)[2],
                          const offset_t (&lay2_enc)[2], const offset_t (&lay2_off)[2],
                          bool add_layers) {
    std::size_t idx = via_list.size();
    via v = via_list.emplace_back(xform, via_id, num, cut_dim, cut_sp, lay1_enc, lay1_off, lay2_enc,
                                  lay2_off);
    if (add_layers) {
        purp_t purpose = tech_ptr->get_purpose_id(nullptr);
        lay_t bot_lay = tech_ptr->get_bot_layer_id(via_id);
        lay_t top_lay = tech_ptr->get_bot_layer_id(via_id);
        rectangle bot_r = v.bot_box();
        rectangle top_r = v.top_box();
        v.set_layer1(add_rect(bot_lay, purpose, bot_r.xl(), bot_r.yl(), bot_r.xh(), bot_r.yh()));
        v.set_layer2(add_rect(top_lay, purpose, top_r.xl(), top_r.yl(), top_r.xh(), top_r.yh()));
    }
    return via_ref(&via_list, idx);
}

} // namespace layout
} // namespace cbag
