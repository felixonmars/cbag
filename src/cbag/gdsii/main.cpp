#include <cbag/gdsii/main.h>
#include <cbag/gdsii/write.h>

#include <cbag/layout/cellview.h>
#include <cbag/layout/via.h>

namespace cbag {
namespace gdsii {

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
                        const layout::geometry &geo) {}

void write_lay_via(spdlog::logger &logger, std::ofstream &stream, const layout::via &v) {}

void write_lay_pin(spdlog::logger &logger, std::ofstream &stream, lay_t lay, purp_t purp,
                   const layout::pin &p, bool make_pin_obj) {}

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
    for (auto iter = cv.begin_via(); iter != cv.end_via(); ++iter) {
        write_lay_via(logger, stream, *iter);
    }

    logger.info("Export layout pins.");
    auto tech_ptr = cv.get_tech();
    auto purp = tech_ptr->pin_purpose;
    auto make_pin_obj = tech_ptr->make_pin_obj;
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
