#ifndef CBAG_UTIL_IO_H
#define CBAG_UTIL_IO_H

#include <filesystem>
#include <fstream>
#include <string>

namespace cbag {
namespace util {

void make_parent_dirs(const std::string &fname);

void make_parent_dirs(const std::filesystem::path &path);

std::ofstream open_file_write(const std::string &fname, bool binary = false);

bool is_file(const std::string &fname);

bool is_file(const std::filesystem::path &path);

std::filesystem::path get_canonical_path(std::string fname);

std::filesystem::path join(const std::string &first, const std::string &second);

} // namespace util
// namespace util
} // namespace cbag
#endif
