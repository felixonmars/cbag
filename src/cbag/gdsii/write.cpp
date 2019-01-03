#include <fmt/core.h>

#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation_util.h>
#include <cbag/gdsii/write.h>
#include <cbag/gdsii/write_util.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/polygon.h>
#include <cbag/layout/via_util.h>
#include <cbag/util/io.h>
#include <cbag/util/string.h>

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

void check_has_next(const util::token_iterator &iter, const std::string &fname) {
    if (!iter.has_next())
        throw std::runtime_error("Cannot parse file: " + fname);
}

uint16_t to_int(std::string &&s, const std::string &fname) {
    try {
        return static_cast<uint16_t>(std::stoi(std::move(s)));
    } catch (const std::invalid_argument &) {
        throw std::runtime_error("Cannot parse file: " + fname);
    } catch (const std::out_of_range &) {
        throw std::runtime_error("Cannot parse file: " + fname);
    }
}

template <typename F> void process_file(const std::string &fname, F fun) {
    auto file = util::open_file_read(fname);
    std::string line;
    while (std::getline(file, line)) {
        // ignore comments
        if (!line.empty() && line[0] != '#') {
            util::token_iterator iter(line, " \t");
            check_has_next(iter, fname);
            auto val1 = iter.get_next();
            check_has_next(iter, fname);
            auto val2 = iter.get_next();
            check_has_next(iter, fname);
            auto lay_id = to_int(iter.get_next(), fname);
            check_has_next(iter, fname);
            auto purp_id = to_int(iter.get_next(), fname);
            fun(val1, val2, lay_id, purp_id);
        }
    }
}

layer_map parse_layer_map(const std::string &fname, const layout::tech &tech) {
    layer_map ans;

    process_file(fname, [&ans, &tech](const std::string &s1, const std::string &s2, uint16_t glay,
                                      uint16_t gpurp) {
        auto lay = tech.get_layer_id(s1);
        auto purp = tech.get_purpose_id(s2);
        if (lay && purp) {
            ans.emplace(std::make_pair(*lay, *purp), std::make_pair(glay, gpurp));
        }
    });

    return ans;
}

boundary_map parse_obj_map(const std::string &fname) {
    boundary_map ans;

    process_file(fname,
                 [&ans](const std::string &s1, const std::string &s2, uint16_t lay, uint16_t purp) {
                     if (s1 == "Boundary") {
                         if (s2 == "PR") {
                             ans.emplace(boundary_type::PR, std::make_pair(lay, purp));
                         } else if (s2 == "snap") {
                             ans.emplace(boundary_type::snap, std::make_pair(lay, purp));
                         }
                     }
                 });

    return ans;
}

gds_lookup::gds_lookup(const layout::tech &tech, const std::string &lay_map_file,
                       const std::string &obj_map_file)
    : lay_map(parse_layer_map(lay_map_file, tech)), bnd_map(parse_obj_map(obj_map_file)) {}

std::optional<gds_layer_t> gds_lookup::get_gds_layer(layer_t key) const {
    auto iter = lay_map.find(key);
    if (iter == lay_map.end())
        return {};
    return iter->second;
}

std::optional<gds_layer_t> get_gds_layer(const gds_lookup &lookup, lay_t lay, purp_t purp) {
    return lookup.get_gds_layer(std::make_pair(lay, purp));
}

std::optional<gds_layer_t> gds_lookup::get_gds_layer(boundary_type bnd_type) const {
    auto iter = bnd_map.find(bnd_type);
    if (iter == bnd_map.end())
        return {};
    return iter->second;
}

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
    auto gkey1 = lookup.get_gds_layer(lay1_key);
    if (!gkey1) {
        logger.warn("Cannot find layer/purpose ({}, {}) in layer map.  Skipping via.",
                    lay1_key.first, lay1_key.second);
        return;
    }
    auto gkey2 = lookup.get_gds_layer(lay2_key);
    if (!gkey2) {
        logger.warn("Cannot find layer/purpose ({}, {}) in layer map.  Skipping via.",
                    lay2_key.first, lay2_key.second);
        return;
    }
    auto gkeyc = lookup.get_gds_layer(cut_key);
    if (!gkeyc) {
        logger.warn("Cannot find layer/purpose ({}, {}) in layer map.  Skipping via.",
                    cut_key.first, cut_key.second);
        return;
    }
    auto [glay1, gpurp1] = *gkey1;
    auto [gcl, gcp] = *gkeyc;
    auto [glay2, gpurp2] = *gkey2;
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

    write_text(logger, stream, lay, purp, pin.get_label(), xform);
    if (make_pin_obj) {
        write_box(logger, stream, lay, purp, pin);
    }
}

void write_lay_label(spdlog::logger &logger, std::ofstream &stream, const layout::label &lab) {
    auto [lay, purp] = lab.get_key();
    write_text(logger, stream, lay, purp, lab.get_text(), lab.get_xform());
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
    for (auto iter = cv.begin_geometry(); iter != cv.end_geometry(); ++iter) {
        auto &[layer_key, geo] = *iter;
        auto gkey = lookup.get_gds_layer(layer_key);
        if (!gkey) {
            logger.warn("Cannot find layer/purpose ({}, {}) in layer map.  Skipping geometry.",
                        layer_key.first, layer_key.second);
        } else {
            auto [glay, gpurp] = *gkey;
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
        auto gkey = get_gds_layer(lookup, lay, purp);
        if (!gkey) {
            logger.warn("Cannot find layer/purpose ({}, {}) in layer map.  Skipping pins.", lay,
                        purp);
        } else {
            auto [glay, gpurp] = *gkey;
            for (const auto &pin : pin_list) {
                write_lay_pin(logger, stream, glay, gpurp, pin, make_pin_obj);
            }
        }
    }

    logger.info("Export layout labels.");
    for (auto iter = cv.begin_label(); iter != cv.end_label(); ++iter) {
        write_lay_label(logger, stream, *iter);
    }

    logger.info("Export layout boundaries.");
    for (auto iter = cv.begin_boundary(); iter != cv.end_boundary(); ++iter) {
        auto btype = iter->get_type();
        auto gkey = lookup.get_gds_layer(btype);
        if (!gkey) {
            logger.warn("Cannot find boundary type {} in object map.  Skipping boundary.", btype);
        } else {
            auto [glay, gpurp] = *gkey;
            write_polygon(logger, stream, glay, gpurp, *iter);
        }
    }

    write_struct_end(logger, stream);

    logger.info("Finish GDS export.");
}

} // namespace gdsii
} // namespace cbag
