
#ifndef CBAG_GDSII_WRITE_H
#define CBAG_GDSII_WRITE_H

#include <fstream>
#include <string>

#include <cbag/logging/logging.h>

#include <cbag/util/sfinae.h>

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
    ENDEL = 0x1100,
};

using size_type = uint16_t;

constexpr auto max_size = UINT16_MAX;
constexpr auto type_size = sizeof(record_type);
constexpr auto size_size = sizeof(size_type);
constexpr auto version = static_cast<int16_t>(5);

void write_header(spdlog::logger &logger, std::ofstream &stream);

void write_units(spdlog::logger &logger, std::ofstream &stream, double resolution,
                 double user_unit);

template <typename T, util::IsUInt<T> = 0> void write_bytes(std::ofstream &stream, T val) {
    constexpr auto unit_size = sizeof(T);
    for (std::size_t bidx = 0, shft = (unit_size - 1) * 8; bidx < unit_size; ++bidx, shft -= 8) {
        auto tmp = static_cast<char>(val >> shft);
        stream.put(tmp);
    }
}

template <typename T, record_type R> void write(std::ofstream &stream, const std::vector<T> &vec) {
    constexpr auto unit_size = sizeof(T);

    auto num_data = vec.size();
    auto size_test = unit_size * num_data + size_size + type_size;
    assert(size_test <= max_size);

    auto size = static_cast<size_type>(size_test);
    bool add_zero = false;
    if (size % 2 == 1) {
        ++size;
        add_zero = true;
    }
    write_bytes(stream, size);
    write_bytes(stream, static_cast<uint16_t>(R));
    for (auto val : vec) {
        write_bytes(stream, val);
    }
    if (add_zero) {
        stream.put('\0');
    }
}

template <record_type R>
void write_struct_begin(spdlog::logger &logger, std::ofstream &stream,
                        const std::vector<uint16_t> &time_vec) {
    std::vector<uint16_t> data(time_vec.begin(), time_vec.end());
    data.insert(data.end(), time_vec.begin(), time_vec.end());
    write<uint16_t, R>(stream, data);
}

template <record_type R> void write_empty(spdlog::logger &logger, std::ofstream &stream) {
    std::vector<uint16_t> data;
    write<uint16_t, R>(stream, data);
}

template <record_type R>
void write_name(spdlog::logger &logger, std::ofstream &stream, const std::string &name) {
    std::vector<unsigned char> data(name.begin(), name.end());
    write<unsigned char, R>(stream, data);
}

} // namespace gdsii
} // namespace cbag

#endif
