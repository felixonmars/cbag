
#ifndef CBAG_GDSII_WRITE_H
#define CBAG_GDSII_WRITE_H

#include <fstream>
#include <string>

namespace cbag {
namespace gdsii {

void write_header(std::ofstream &stream);

void write_bgn_lib(std::ofstream &stream);

void write_lib_name(std::ofstream &stream, const std::string &name);

void write_units(std::ofstream &stream, double resolution, double user_unit);

void write_end_lib(std::ofstream &stream);

} // namespace gdsii
} // namespace cbag

#endif
