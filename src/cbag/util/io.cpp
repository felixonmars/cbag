#include <boost/filesystem.hpp>

#include <cbag/util/io.h>

namespace cbag {
namespace util {

void make_parent_dirs(const std::string &fname) {
    boost::filesystem::path path(fname);
    if (path.has_parent_path()) {
        boost::filesystem::create_directories(path.parent_path());
    }
}

std::ofstream open_file_write(const std::string &fname) {
    make_parent_dirs(fname);
    return std::ofstream(fname, std::ios_base::out);
}

} // namespace util
} // namespace cbag
