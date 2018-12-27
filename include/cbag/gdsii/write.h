
#ifndef CBAG_GDSII_WRITE_H
#define CBAG_GDSII_WRITE_H

#include <fstream>
#include <string>

#include <cbag/common/box_t.h>
#include <cbag/common/transformation.h>
#include <cbag/layout/polygon_fwd.h>
#include <cbag/logging/logging.h>

namespace cbag {
namespace gdsii {

void write_header(spdlog::logger &logger, std::ofstream &stream);

void write_units(spdlog::logger &logger, std::ofstream &stream, double resolution,
                 double user_unit);

void write_lib_begin(spdlog::logger &logger, std::ofstream &stream,
                     const std::vector<uint16_t> &time_vec);

void write_lib_name(spdlog::logger &logger, std::ofstream &stream, const std::string &name);

void write_lib_end(spdlog::logger &logger, std::ofstream &stream);

void write_struct_begin(spdlog::logger &logger, std::ofstream &stream,
                        const std::vector<uint16_t> &time_vec);

void write_struct_name(spdlog::logger &logger, std::ofstream &stream, const std::string &name);

void write_struct_end(spdlog::logger &logger, std::ofstream &stream);

void write_polygon(spdlog::logger &logger, std::ofstream &stream, lay_t layer, purp_t purpose,
                   const layout::polygon &poly);

void write_box(spdlog::logger &logger, std::ofstream &stream, lay_t layer, purp_t purpose,
               const box_t &box);

void write_instance(spdlog::logger &logger, std::ofstream &stream, const std::string &cell_name,
                    const transformation &xform, cnt_t nx = 1, cnt_t ny = 1, offset_t spx = 0,
                    offset_t spy = 0);

void write_text(spdlog::logger &logger, std::ofstream &stream, lay_t layer, purp_t purpose,
                const std::string &text, const transformation &xform);

} // namespace gdsii
} // namespace cbag

#endif
