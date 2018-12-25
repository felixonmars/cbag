#include <cassert>
#include <cstdint>
#include <vector>

#include <cbag/gdsii/write.h>
#include <cbag/util/io.h>

namespace cbag {
namespace gdsii {

using size_type = uint16_t;

enum class record_type : uint16_t {
    HEADER = 0x0002,
    BGNLIB = 0x0102,
    LIBNAME = 0x0206,
    UNITS = 0x0305,
    ENDLIB = 0x0400,
};

constexpr auto max_size = UINT16_MAX;
constexpr auto type_size = sizeof(record_type);
constexpr auto size_size = sizeof(size_type);
constexpr auto version = static_cast<int16_t>(5);

template <typename T>
void write(std::ofstream &stream, record_type rtype, const std::vector<T> &vec) {
    constexpr auto unit_size = sizeof(T);

    auto size_test = unit_size * vec.size() + size_size + type_size;
    if (size_test > max_size)
        throw std::runtime_error("Maximum GDS record size exceeded.");

    auto size = static_cast<size_type>(size_test);
    bool add_zero = false;
    if (size % 2 == 1) {
        ++size;
        add_zero = true;
    }
    stream.write(reinterpret_cast<char *>(&size), size_size);
    stream.write(reinterpret_cast<char *>(&rtype), type_size);
    if (!vec.empty()) {
        stream.write(reinterpret_cast<const char *>(vec.data()), sizeof(T) * vec.size());
        if (add_zero) {
            char zero = '\0';
            stream.write(&zero, 1);
        }
    }
}

void write_header(std::ofstream &stream) {
    std::vector<uint16_t> data({version});
    write(stream, record_type::HEADER, data);
}

std::vector<uint16_t> get_gds_time() { return {0, 0, 0, 0, 0, 0}; }

void write_bgn_lib(std::ofstream &stream) {
    auto time_vec = get_gds_time();
    decltype(time_vec) data(time_vec.begin(), time_vec.end());
    data.insert(data.end(), time_vec.begin(), time_vec.end());
    write(stream, record_type::BGNLIB, data);
}

void write_lib_name(std::ofstream &stream, const std::string &name) {
    std::vector<char> data(name.begin(), name.end());
    write(stream, record_type::LIBNAME, data);
}

void write_units(std::ofstream &stream, double resolution, double user_unit) {
    assert(sizeof(double) == 8);
    std::vector<double> data({resolution / user_unit, resolution});
    write(stream, record_type::UNITS, data);
}

void write_end_lib(std::ofstream &stream) {
    std::vector<uint16_t> data;
    write(stream, record_type::ENDLIB, data);
}

} // namespace gdsii
} // namespace cbag
