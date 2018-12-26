
#ifndef CBAG_GDSII_WRITE_H
#define CBAG_GDSII_WRITE_H

#include <fstream>
#include <string>

#include <cbag/logging/logging.h>

namespace cbag {
namespace gdsii {

enum class record_type : uint16_t {
    HEADER = 0x0002,
    BGNLIB = 0x0102,
    LIBNAME = 0x0206,
    UNITS = 0x0305,
    ENDLIB = 0x0400,
    BGNSTR = 0x0502,
    STRNAME = 0x0606,
    ENDSTR = 0x0700,
};

void write_header(spdlog::logger &logger, std::ofstream &stream);

void write_begin(spdlog::logger &logger, std::ofstream &stream, record_type rec_type,
                 const std::vector<uint16_t> &time_vec);

void write_name(spdlog::logger &logger, std::ofstream &stream, record_type rec_type,
                const std::string &name);

void write_units(spdlog::logger &logger, std::ofstream &stream, double resolution,
                 double user_unit);

void write_end(spdlog::logger &logger, std::ofstream &stream, record_type rec_type);

} // namespace gdsii
} // namespace cbag

#endif
