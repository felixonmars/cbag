
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

using size_type = uint16_t;

enum class record_type : uint16_t {
    HEADER = 0x0002,
    BGNLIB = 0x0102,
    LIBNAME = 0x0206,
    UNITS = 0x0305,
};

constexpr auto max_size = UINT16_MAX;
constexpr auto type_size = sizeof(record_type);
constexpr auto size_size = sizeof(size_type);

template <typename T, typename = util::IsInt<T>>
void write(std::fstream &stream, record_type rtype, const std::vector<T> &vec) {
    constexpr auto unit_size = sizeof(T);

    auto size_test = unit_size * vec.size() + size_size + type_size;
    if (size_test > max_size)
        throw std::runtime_error("Maximum GDS record size exceeded.");

    auto size = static_cast<size_type>(size_test);
    stream.write(reinterpret_cast<char *>(&size), size_size);
    stream.write(reinterpret_cast<char *>(&rtype), type_size);
    stream.write(reinterpret_cast<char *>(vec.data()), sizeof(T) * vec.size());
}

} // namespace gdsii
} // namespace cbag

#endif
