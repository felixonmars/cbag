#include <fmt/core.h>

#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation_util.h>
#include <cbag/gdsii/main.h>
#include <cbag/gdsii/write.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/polygon.h>
#include <cbag/layout/via_util.h>

namespace cbag {
namespace gdsii {

class polygon_writer {
  public:
    using value_type = layout::polygon;

  private:
    spdlog::logger &logger;
    std::ofstream &stream;
    glay_t layer;
    gpurp_t purpose;
    value_type last;

  public:
    polygon_writer(spdlog::logger &logger, std::ofstream &stream, glay_t layer, gpurp_t purpose)
        : logger(logger), stream(stream), layer(layer), purpose(purpose) {}

    void push_back(value_type &&v) {
        record_last();
        last = std::move(v);
    }

    void insert(value_type *ptr, const value_type &v) {
        record_last();
        last = v;
    }

    void record_last() const {
        if (last.size() > 0) {
            write_polygon(logger, stream, layer, purpose, last);
        }
    }

    value_type &back() { return last; }

    value_type *end() const { return nullptr; }
};

class rect_writer {
  private:
    spdlog::logger &logger;
    std::ofstream &stream;
    glay_t layer;
    gpurp_t purpose;

  public:
    rect_writer(spdlog::logger &logger, std::ofstream &stream, glay_t layer, gpurp_t purpose)
        : logger(logger), stream(stream), layer(layer), purpose(purpose) {}

    rect_writer &operator=(const box_t &box) {
        write_box(logger, stream, layer, purpose, box);
        return *this;
    }

    rect_writer &operator*() { return *this; }

    rect_writer &operator++() { return *this; }
};

layer_map parse_layer_map(const std::string &fname) {
    layer_map ans;

    // TODO: implement this

    return ans;
}

boundary_map parse_obj_map(const std::string &fname) {
    boundary_map ans;

    // TODO: implement this

    return ans;
}

gds_lookup::gds_lookup(const std::string &lay_map_file, const std::string &obj_map_file)
    : lay_map(parse_layer_map(lay_map_file)), bnd_map(parse_obj_map(obj_map_file)) {}

layer_map::const_iterator gds_lookup::find_gds_layer(const layer_t &key) const {
    return lay_map.find(key);
}

layer_map::const_iterator find_gds_layer(const gds_lookup &lookup, lay_t lay, purp_t purp) {
    return lookup.find_gds_layer(std::make_pair(lay, purp));
}

boundary_map::const_iterator gds_lookup::find_gds_layer(boundary_type bnd_type) const {
    return bnd_map.find(bnd_type);
}

layer_map::const_iterator gds_lookup::end_layer() const { return lay_map.end(); }

boundary_map::const_iterator gds_lookup::end_boundary() const { return bnd_map.end(); }

std::vector<tval_t> get_gds_time() {
    auto ep_time = std::time(nullptr);
    auto loc_time = std::localtime(&ep_time);
    return {
        static_cast<tval_t>(loc_time->tm_year), static_cast<tval_t>(loc_time->tm_mon + 1),
        static_cast<tval_t>(loc_time->tm_mday), static_cast<tval_t>(loc_time->tm_hour),
        static_cast<tval_t>(loc_time->tm_min),  static_cast<tval_t>(loc_time->tm_sec),
    };
}

void write_gds_start(spdlog::logger &logger, std::ofstream &stream, const std::string &lib_name,
                     double resolution, double user_unit, const std::vector<tval_t> &time_vec) {
    write_header(logger, stream);
    write_lib_begin(logger, stream, time_vec);
    write_lib_name(logger, stream, lib_name);
    write_units(logger, stream, resolution, user_unit);
}

void write_gds_stop(spdlog::logger &logger, std::ofstream &stream) {
    write_lib_end(logger, stream);
    stream.close();
}

void write_lay_geometry(spdlog::logger &logger, std::ofstream &stream, glay_t lay, gpurp_t purp,
                        const layout::geometry &geo) {
    polygon_writer w(logger, stream, lay, purp);
    geo.write_geometry(w);
    w.record_last();
}

void write_lay_via(spdlog::logger &logger, std::ofstream &stream, const layout::tech &tech,
                   const gds_lookup &lookup, const layout::via &v) {
    auto [lay1_key, cut_key, lay2_key] = tech.get_via_layer_purpose(v.get_via_id());
    auto iter1 = lookup.find_gds_layer(lay1_key);
    auto iter2 = lookup.find_gds_layer(lay2_key);
    auto iterc = lookup.find_gds_layer(cut_key);
    auto itere = lookup.end_layer();
    if (iter1 == itere) {
        logger.warn("Cannot find layer/purpose ({}, {}) in layer map.  Skipping via.",
                    lay1_key.first, lay1_key.second);
        return;
    }
    if (iter2 == itere) {
        logger.warn("Cannot find layer/purpose ({}, {}) in layer map.  Skipping via.",
                    lay2_key.first, lay2_key.second);
        return;
    }
    if (iterc == itere) {
        logger.warn("Cannot find layer/purpose ({}, {}) in layer map.  Skipping via.",
                    cut_key.first, cut_key.second);
        return;
    }
    auto [glay1, gpurp1] = iter1->second;
    auto [gcl, gcp] = iterc->second;
    auto [glay2, gpurp2] = iter2->second;
    write_box(logger, stream, glay1, gpurp1, layout::get_bot_box(v));
    write_box(logger, stream, glay2, gpurp2, layout::get_top_box(v));
    get_via_cuts(v, rect_writer(logger, stream, gcl, gcp));
}

void write_lay_pin(spdlog::logger &logger, std::ofstream &stream, glay_t lay, gpurp_t purp,
                   const layout::pin &pin, bool make_pin_obj) {
    if (!is_physical(pin)) {
        logger.warn("non-physical bbox {} on pin layer ({}, {}), skipping.", to_string(pin), lay,
                    purp);
        return;
    }
    auto xc = xm(pin);
    auto yc = ym(pin);
    auto w = width(pin);
    auto text_h = height(pin);
    transformation xform;
    if (text_h > w) {
        xform = make_xform(xc, yc, oR90);
        text_h = w;
    } else {
        xform = make_xform(xc, yc, oR0);
    }

    write_text(logger, stream, lay, purp, pin.label, xform);
    if (make_pin_obj) {
        write_box(logger, stream, lay, purp, pin);
    }
}

void write_lay_cellview(spdlog::logger &logger, std::ofstream &stream, const std::string &cell_name,
                        const cbag::layout::cellview &cv,
                        const std::unordered_map<std::string, std::string> &rename_map,
                        const std::vector<tval_t> &time_vec, const gds_lookup &lookup) {
    write_struct_begin(logger, stream, time_vec);
    write_struct_name(logger, stream, cell_name);

    logger.info("Export layout instances.");
    for (auto iter = cv.begin_inst(); iter != cv.end_inst(); ++iter) {
        auto &[inst_name, inst] = *iter;
        write_instance(logger, stream, inst.get_cell_name(&rename_map), inst.xform, inst.nx,
                       inst.ny, inst.spx, inst.spy);
    }

    logger.info("Export layout geometries.");
    auto iterel = lookup.end_layer();
    for (auto iter = cv.begin_geometry(); iter != cv.end_geometry(); ++iter) {
        auto &[layer_key, geo] = *iter;
        auto iterl = lookup.find_gds_layer(layer_key);
        if (iterl == iterel) {
            logger.warn("Cannot find layer/purpose ({}, {}) in layer map.  Skipping geometry.",
                        layer_key.first, layer_key.second);
        } else {
            auto [glay, gpurp] = iterl->second;
            write_lay_geometry(logger, stream, glay, gpurp, geo);
        }
    }

    logger.info("Export layout vias.");
    auto tech_ptr = cv.get_tech();
    for (auto iter = cv.begin_via(); iter != cv.end_via(); ++iter) {
        write_lay_via(logger, stream, *tech_ptr, lookup, *iter);
    }

    logger.info("Export layout pins.");
    auto purp = tech_ptr->get_pin_purpose();
    auto make_pin_obj = tech_ptr->get_make_pin();
    for (auto iter = cv.begin_pin(); iter != cv.end_pin(); ++iter) {
        auto &[lay, pin_list] = *iter;
        auto iterl = find_gds_layer(lookup, lay, purp);
        if (iterl == iterel) {
            logger.warn("Cannot find layer/purpose ({}, {}) in layer map.  Skipping pins.", lay,
                        purp);
        } else {
            auto [glay, gpurp] = iterl->second;
            for (const auto &pin : pin_list) {
                write_lay_pin(logger, stream, glay, gpurp, pin, make_pin_obj);
            }
        }
    }

    logger.info("Export layout boundaries.");
    auto itereb = lookup.end_boundary();
    for (auto iter = cv.begin_boundary(); iter != cv.end_boundary(); ++iter) {
        auto btype = iter->get_type();
        auto iterb = lookup.find_gds_layer(btype);
        if (iterb == itereb) {
            logger.warn("Cannot find boundary type {} in object map.  Skipping boundary.", btype);
        } else {
            auto [glay, gpurp] = iterb->second;
            write_polygon(logger, stream, glay, gpurp, *iter);
        }
    }

    write_struct_end(logger, stream);
}

} // namespace gdsii
} // namespace cbag
