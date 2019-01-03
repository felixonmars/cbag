#include <cbag/util/io.h>

namespace cbag {
namespace util {

void make_parent_dirs(const std::string &fname) { make_parent_dirs(std::filesystem::path(fname)); }

void make_parent_dirs(const std::filesystem::path &path) {
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
}

std::ofstream open_file_write(const std::string &fname, bool binary) {
    make_parent_dirs(fname);
    auto mode = std::ios_base::out;
    if (binary)
        mode |= std::ios_base::binary;
    std::ofstream ans{fname, mode};
    if (binary)
        ans.imbue(std::locale::classic());
    return ans;
}

std::ifstream open_file_read(const std::string &fname, bool binary) {
    if (!is_file(fname))
        throw std::invalid_argument(fname + " is not a file.");
    auto mode = std::ios_base::out;
    if (binary)
        mode |= std::ios_base::binary;
    std::ifstream ans{fname, mode};
    if (binary)
        ans.imbue(std::locale::classic());
    return ans;
}

bool is_file(const std::string &fname) { return is_file(std::filesystem::path(fname)); }

bool is_file(const std::filesystem::path &path) { return std::filesystem::is_regular_file(path); }

std::filesystem::path get_canonical_path(std::string fname) {
    return std::filesystem::canonical(std::filesystem::path(std::move(fname)));
}

std::filesystem::path join(const std::string &first, const std::string &second) {
    std::filesystem::path ans(first);
    ans /= second;
    return ans;
}

} // namespace util
} // namespace cbag
