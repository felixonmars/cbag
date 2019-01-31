
#ifndef CBAG_GDSII_WRITE_UTIL_H
#define CBAG_GDSII_WRITE_UTIL_H

#include <fstream>
#include <string>

#include <cbag/common/box_t.h>
#include <cbag/common/transformation.h>
#include <cbag/gdsii/typedefs.h>
#include <cbag/layout/polygon_fwd.h>
#include <cbag/logging/logging.h>
#include <cbag/util/sfinae.h>

namespace cbag {
namespace gdsii {

template <typename T, util::IsUInt<T> = 0> void write_bytes(std::ostream &stream, T val) {
    constexpr auto unit_size = sizeof(T);
    for (std::size_t bidx = 0, shft = (unit_size - 1) * 8; bidx < unit_size; ++bidx, shft -= 8) {
        auto tmp = static_cast<char>((val >> shft) & 0xff);
        stream.put(tmp);
    }
}

void write_header(spdlog::logger &logger, std::ostream &stream);

void write_units(spdlog::logger &logger, std::ostream &stream, double resolution, double user_unit);

void write_lib_begin(spdlog::logger &logger, std::ostream &stream,
                     const std::vector<tval_t> &time_vec);

void write_lib_name(spdlog::logger &logger, std::ostream &stream, const std::string &name);

void write_lib_end(spdlog::logger &logger, std::ostream &stream);

void write_struct_begin(spdlog::logger &logger, std::ostream &stream,
                        const std::vector<tval_t> &time_vec);

void write_struct_name(spdlog::logger &logger, std::ostream &stream, const std::string &name);

void write_struct_end(spdlog::logger &logger, std::ostream &stream);

void write_transform(spdlog::logger &logger, std::ostream &stream, const transformation &xform,
                     double mag = 1.0, cnt_t nx = 1, cnt_t ny = 1, offset_t spx = 0,
                     offset_t spy = 0);

void write_polygon(spdlog::logger &logger, std::ostream &stream, glay_t layer, gpurp_t purpose,
                   const layout::polygon &poly);

void write_box(spdlog::logger &logger, std::ostream &stream, glay_t layer, gpurp_t purpose,
               const box_t &box);

void write_instance(spdlog::logger &logger, std::ostream &stream, const std::string &cell_name,
                    const std::string &inst_name, const transformation &xform, cnt_t nx = 1,
                    cnt_t ny = 1, offset_t spx = 0, offset_t spy = 0);

void write_text(spdlog::logger &logger, std::ostream &stream, glay_t layer, gpurp_t purpose,
                const std::string &text, const transformation &xform, offset_t height,
                double resolution);

} // namespace gdsii
} // namespace cbag

#endif
