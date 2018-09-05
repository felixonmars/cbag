
#include <fmt/format.h>

#include <cbag/util/binary_iterator.h>

#include <cbag/layout/blockage.h>
#include <cbag/layout/boundary.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/geometry.h>
#include <cbag/layout/rectangle.h>
#include <cbag/layout/via.h>

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

cellview::cellview(std::string tech, uint8_t geo_mode)
    : tech(std::move(tech)), geo_mode(geo_mode) {}

rectangle cellview::get_bbox(lay_t lay_id, purp_t purp_id) const {
    auto iter = geo_map.find(layer_t(lay_id, purp_id));
    if (iter == geo_map.end()) {
        return rectangle(0, 0, -1, -1);
    }
    return iter->second.get_bbox();
}

polygon_ref<rectangle> cellview::add_rect(const layer_t &layer, coord_t xl, coord_t yl, coord_t xh,
                                          coord_t yh) {
    auto iter = helper::get_geometry(*this, layer);
    return iter->second.add_rect(xl, yl, xh, yh);
}

polygon_ref<polygon90> cellview::add_poly90(const layer_t &layer, pt_vector data) {
    auto iter = geo_map.find(layer);
    if (iter == geo_map.end()) {
        iter = geo_map.emplace(layer, geometry(geo_mode)).first;
    }
    return iter->second.add_poly90(std::move(data));
}

polygon_ref<polygon45> cellview::add_poly45(const layer_t &layer, pt_vector data) {
    auto iter = geo_map.find(layer);
    if (iter == geo_map.end()) {
        iter = geo_map.emplace(layer, geometry(geo_mode)).first;
    }
    return iter->second.add_poly45(std::move(data));
}

polygon_ref<polygon> cellview::add_poly(const layer_t &layer, pt_vector data) {
    auto iter = geo_map.find(layer);
    if (iter == geo_map.end()) {
        iter = geo_map.emplace(layer, geometry(geo_mode)).first;
    }
    return iter->second.add_poly(std::move(data));
}

void cellview::add_path_seg(const layer_t &layer, coord_t x0, coord_t y0, coord_t x1, coord_t y1,
                            dist_t width, const char *style0, const char *style1) {}

inst_map_t::iterator cellview::add_prim_instance(const char *lib, const char *cell,
                                                 const char *view, const char *name,
                                                 transformation xform, uint32_t nx, uint32_t ny,
                                                 offset_t spx, offset_t spy) {
    std::string key = helper::get_inst_name(*this, name);

    return inst_map
        .emplace(helper::get_inst_name(*this, name),
                 instance(lib, cell, view, std::move(xform), nx, ny, spx, spy))
        .first;
}

inst_map_t::iterator cellview::add_instance(const cellview *cv, const char *name,
                                            transformation xform, uint32_t nx, uint32_t ny,
                                            offset_t spx, offset_t spy) {
    std::string key = helper::get_inst_name(*this, name);

    return inst_map
        .emplace(helper::get_inst_name(*this, name),
                 instance(cv, std::move(xform), nx, ny, spx, spy))
        .first;
}

} // namespace layout
} // namespace cbag
