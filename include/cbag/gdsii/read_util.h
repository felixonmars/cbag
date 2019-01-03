
#ifndef CBAG_GDSII_READ_UTIL_H
#define CBAG_GDSII_READ_UTIL_H

#include <fstream>
#include <string>

#include <cbag/logging/logging.h>

namespace cbag {
namespace gdsii {

void read_header(spdlog::logger &logger, std::ifstream &stream);

void read_lib_begin(spdlog::logger &logger, std::ifstream &stream);

std::string read_lib_name(spdlog::logger &logger, std::ifstream &stream);

void read_units(spdlog::logger &logger, std::ifstream &stream);

} // namespace gdsii
} // namespace cbag

#endif
