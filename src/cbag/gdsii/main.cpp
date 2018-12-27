#include <cbag/gdsii/main.h>
#include <cbag/gdsii/write.h>

#include <cbag/layout/cellview.h>

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
    write_struct_begin<record_type::BGNLIB>(logger, stream, time_vec);
    write_name<record_type::LIBNAME>(logger, stream, lib_name);
    write_units(logger, stream, resolution, user_unit);
}

void write_gds_stop(spdlog::logger &logger, std::ofstream &stream) {
    write_empty<record_type::ENDLIB>(logger, stream);
    stream.close();
}

void write_lay_cellview(spdlog::logger &logger, std::ofstream &stream, const std::string &cell_name,
                        const cbag::layout::cellview &cv,
                        const std::unordered_map<std::string, std::string> &rename_map,
                        const std::vector<uint16_t> &time_vec) {
    write_struct_begin<record_type::BGNSTR>(logger, stream, time_vec);
    write_name<record_type::STRNAME>(logger, stream, cell_name);
    write_empty<record_type::ENDSTR>(logger, stream);
}

} // namespace gdsii
} // namespace cbag
