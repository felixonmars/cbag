
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

cellview::cellview(std::string tech, uint8_t geo_mode)
    : tech(std::move(tech)), inst_name_cnt(0), geo_mode(geo_mode) {}

rectangle cellview::get_bbox(const layer_t &layer) const {
    auto iter = geo_map.find(layer);
    if (iter == geo_map.end()) {
        return rectangle();
    }
    return iter->second.get_bbox();
}

std::string cellview::add_instance(instance inst, const char *name) {
    std::string inst_name;
    if (name == nullptr) {
        inst_name = get_inst_name();
    } else {
        inst_name = name;
        if (inst_map.find(inst_name) != inst_map.end()) {
            inst_name = get_inst_name();
        }
    }

    inst_map.emplace(inst_name, std::move(inst));
    return inst_name;
}

polygon_ref<rectangle> cellview::add_rect(const layer_t &layer, coord_t xl, coord_t yl, coord_t xh,
                                          coord_t yh) {
    auto iter = get_geometry(layer);
    return iter->second.add_rect(xl, yl, xh, yh);
}

polygon_ref<polygon90> cellview::add_poly90(const layer_t &layer, point_vector_t data) {
    auto iter = geo_map.find(layer);
    if (iter == geo_map.end()) {
        iter = geo_map.emplace(layer, geometry(geo_mode)).first;
    }
    return iter->second.add_poly90(std::move(data));
}

polygon_ref<polygon45> cellview::add_poly45(const layer_t &layer, point_vector_t data) {
    auto iter = geo_map.find(layer);
    if (iter == geo_map.end()) {
        iter = geo_map.emplace(layer, geometry(geo_mode)).first;
    }
    return iter->second.add_poly45(std::move(data));
}

polygon_ref<polygon> cellview::add_poly(const layer_t &layer, point_vector_t data) {
    auto iter = geo_map.find(layer);
    if (iter == geo_map.end()) {
        iter = geo_map.emplace(layer, geometry(geo_mode)).first;
    }
    return iter->second.add_poly(std::move(data));
}

void cellview::add_path_seg(const layer_t &layer, coord_t x0, coord_t y0, coord_t x1, coord_t y1,
                            dist_t width, const char *style0, const char *style1) {}


std::string cellview::get_inst_name() {
    cbag::util::binary_iterator<uint32_t> iter(inst_name_cnt, std::optional<uint32_t>());
    while (iter.has_next()) {
        if (inst_map.find(fmt::format("X{d}", *iter)) == inst_map.end()) {
            iter.save();
            iter.down();
        } else {
            iter.up();
        }
    }

    inst_name_cnt = *(iter.get_save());
    return fmt::format("X{d}", inst_name_cnt);
}

geo_map_t::iterator cellview::get_geometry(const layer_t &layer) {
    geo_map_t::iterator iter = geo_map.find(layer);
    if (iter == geo_map.end()) {
        iter = geo_map.emplace(layer, geometry(geo_mode)).first;
    }
    return iter;
}

} // namespace layout
} // namespace cbag
