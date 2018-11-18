#ifndef CBAG_UTIL_IO_H
#define CBAG_UTIL_IO_H

#include <fstream>
#include <string>

namespace cbag {
namespace util {

void make_parent_dirs(const std::string &fname);

std::ofstream open_file_write(const std::string &fname);
} // namespace util
// namespace util
} // namespace cbag
#endif
