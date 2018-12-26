#include <cassert>
#include <cstdint>
#include <vector>

#include <fmt/core.h>

#include <cbag/gdsii/math.h>
#include <cbag/gdsii/write.h>
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
    ENDLIB = 0x0400,
};

constexpr auto max_size = UINT16_MAX;
constexpr auto type_size = sizeof(record_type);
constexpr auto size_size = sizeof(size_type);
constexpr auto version = static_cast<int16_t>(5);

template <typename T, util::IsUInt<T> = 0> void write_bytes(std::ofstream &stream, T val) {
    constexpr auto unit_size = sizeof(T);
    for (std::size_t bidx = 0, shft = (unit_size - 1) * 8; bidx < unit_size; ++bidx, shft -= 8) {
        auto tmp = static_cast<char>(val >> shft);
        stream.put(tmp);
    }
}

template <typename T>
void write(std::ofstream &stream, record_type rtype, const std::vector<T> &vec) {
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
    write_bytes(stream, static_cast<uint16_t>(rtype));
    for (auto val : vec) {
        write_bytes(stream, val);
    }
    if (add_zero) {
        stream.put('\0');
    }
}

void write_header(spdlog::logger &logger, std::ofstream &stream) {
    std::vector<uint16_t> data({version});
    write(stream, record_type::HEADER, data);
}

void write_bgn_lib(spdlog::logger &logger, std::ofstream &stream) {
    auto time_vec = get_gds_time();
    decltype(time_vec) data(time_vec.begin(), time_vec.end());
    data.insert(data.end(), time_vec.begin(), time_vec.end());
    write(stream, record_type::BGNLIB, data);
}

void write_lib_name(spdlog::logger &logger, std::ofstream &stream, const std::string &name) {
    std::vector<unsigned char> data(name.begin(), name.end());
    write(stream, record_type::LIBNAME, data);
}

void write_units(spdlog::logger &logger, std::ofstream &stream, double resolution,
                 double user_unit) {
    std::vector<uint64_t> data(
        {convert_double(resolution), convert_double(resolution * user_unit)});
    write(stream, record_type::UNITS, data);
}

void write_end_lib(spdlog::logger &logger, std::ofstream &stream) {
    std::vector<uint16_t> data;
    write(stream, record_type::ENDLIB, data);
}

} // namespace gdsii
} // namespace cbag
