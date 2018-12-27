#include <cbag/gdsii/main.h>
#include <cbag/gdsii/write.h>

#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation_util.h>
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
    lay_t layer;
    purp_t purpose;
    value_type last;

  public:
    polygon_writer(spdlog::logger &logger, std::ofstream &stream, lay_t layer, purp_t purpose)
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
    lay_t layer;
    purp_t purpose;

  public:
    rect_writer(spdlog::logger &logger, std::ofstream &stream, lay_t layer, purp_t purpose)
        : logger(logger), stream(stream), layer(layer), purpose(purpose) {}

    rect_writer &operator=(const box_t &box) {
        write_box(logger, stream, layer, purpose, box);
        return *this;
    }

    rect_writer &operator*() { return *this; }

    rect_writer &operator++() { return *this; }
};

std::vector<uint16_t> get_gds_time() {
    auto ep_time = std::time(nullptr);
    auto loc_time = std::localtime(&ep_time);
    return {
        static_cast<uint16_t>(loc_time->tm_year), static_cast<uint16_t>(loc_time->tm_mon + 1),
        static_cast<uint16_t>(loc_time->tm_mday), static_cast<uint16_t>(loc_time->tm_hour),
        static_cast<uint16_t>(loc_time->tm_min),  static_cast<uint16_t>(loc_time->tm_sec),
    };
}

void write_gds_start(spdlog::logger &logger, std::ofstream &stream, const std::string &lib_name,
                     double resolution, double user_unit, const std::vector<uint16_t> &time_vec) {
    write_header(logger, stream);
    write_lib_begin(logger, stream, time_vec);
    write_lib_name(logger, stream, lib_name);
    write_units(logger, stream, resolution, user_unit);
}

void write_gds_stop(spdlog::logger &logger, std::ofstream &stream) {
    write_lib_end(logger, stream);
    stream.close();
}

void write_lay_geometry(spdlog::logger &logger, std::ofstream &stream, lay_t lay, purp_t purp,
                        const layout::geometry &geo) {
    polygon_writer w(logger, stream, lay, purp);
    geo.write_geometry(w);
    w.record_last();
}

void write_lay_via(spdlog::logger &logger, std::ofstream &stream, const layout::tech &tech,
                   const layout::via &v) {
    auto [lay1_key, cut_key, lay2_key] = tech.get_via_layer_purpose(v.get_via_id());
    write_box(logger, stream, lay1_key.first, lay1_key.second, layout::get_bot_box(v));
    write_box(logger, stream, lay2_key.first, lay2_key.second, layout::get_top_box(v));
    get_via_cuts(v, rect_writer(logger, stream, cut_key.first, cut_key.second));
}

void write_lay_pin(spdlog::logger &logger, std::ofstream &stream, lay_t lay, purp_t purp,
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
                        const std::vector<uint16_t> &time_vec) {
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
        write_lay_geometry(logger, stream, layer_key.first, layer_key.second, geo);
    }

    logger.info("Export layout vias.");
    auto tech_ptr = cv.get_tech();
    for (auto iter = cv.begin_via(); iter != cv.end_via(); ++iter) {
        write_lay_via(logger, stream, *tech_ptr, *iter);
    }

    logger.info("Export layout pins.");
    auto purp = tech_ptr->get_pin_purpose();
    auto make_pin_obj = tech_ptr->get_make_pin();
    for (auto iter = cv.begin_pin(); iter != cv.end_pin(); ++iter) {
        auto &[lay, pin_list] = *iter;
        for (const auto &pin : pin_list) {
            write_lay_pin(logger, stream, lay, purp, pin, make_pin_obj);
        }
    }

    // TODO: Figure out how to add boundaries and blockages

    write_struct_end(logger, stream);
}

} // namespace gdsii
} // namespace cbag
