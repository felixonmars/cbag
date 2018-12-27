
#ifndef CBAG_GDSII_WRITE_H
#define CBAG_GDSII_WRITE_H

#include <fstream>
#include <string>

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

} // namespace gdsii
} // namespace cbag

#endif
