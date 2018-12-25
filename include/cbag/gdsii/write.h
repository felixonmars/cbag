
#ifndef CBAG_GDSII_WRITE_H
#define CBAG_GDSII_WRITE_H

#include <cstdint>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <cbag/logging/spdlog.h>

#include <cbag/util/io.h>
#include <cbag/util/sfinae.h>

namespace cbag {
namespace gdsii {

void write_header(std::ofstream &stream);

} // namespace gdsii
} // namespace cbag

#endif
