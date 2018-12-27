#include <cassert>
#include <cstdint>
#include <vector>

#include <cbag/gdsii/math.h>
#include <cbag/gdsii/write.h>
#include <cbag/util/io.h>
#include <cbag/util/sfinae.h>

namespace cbag {
namespace gdsii {

class uchar_iter {
  private:
    std::string::const_iterator iter;

  public:
    uchar_iter() = default;
    uchar_iter(std::string::const_iterator iter) : iter(std::move(iter)) {}

    unsigned char operator*() { return static_cast<unsigned char>(*iter); }
    uchar_iter &operator++() {
        ++iter;
        return *this;
    }
    bool operator!=(const uchar_iter &other) { return iter != other.iter; }
};

enum class record_type : uint16_t {
    HEADER = 0x0002,
    BGNLIB = 0x0102,
    LIBNAME = 0x0206,
    UNITS = 0x0305,
    ENDLIB = 0x0400,
    BGNSTR = 0x0502,
    STRNAME = 0x0606,
    ENDSTR = 0x0700,
    BOUNDARY = 0x0800,
    SREF = 0x0A00,
    AREF = 0x0B00,
    TEXT = 0x0C00,
    LAYER = 0x0D02,
    DATATYPE = 0x0E02,
    WIDTH = 0x0F03,
    XY = 0x1003,
    ENDEL = 0x1100,
    SNAME = 0x1206,
    COLROW = 0x1302,
    TEXTTYPE = 0x1602,
    PRESENTATION = 0x1701,
    STRING = 0x1906,
    STRANS = 0x1A01,
    ANGLE = 0x1C05,
    BOX = 0x2D00,
    BOXTYPE = 0x2E02,
};

using size_type = uint16_t;

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

template <record_type R, typename iT>
void write(std::ofstream &stream, std::size_t num_data, iT start, iT stop) {
    constexpr auto unit_size = sizeof(*start);

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
    for (; start != stop; ++start) {
        write_bytes(stream, *start);
    }
    if (add_zero) {
        stream.put('\0');
    }
}

template <record_type R>
void write_grp_begin(spdlog::logger &logger, std::ofstream &stream,
                     const std::vector<uint16_t> &time_vec) {
    std::vector<uint16_t> data(time_vec.begin(), time_vec.end());
    data.insert(data.end(), time_vec.begin(), time_vec.end());
    write<R>(stream, data.size(), data.begin(), data.end());
}

template <record_type R> void write_empty(spdlog::logger &logger, std::ofstream &stream) {
    uint16_t tmp[1] = {0};
    write<R>(stream, 0, tmp, tmp);
}

template <record_type R>
void write_name(spdlog::logger &logger, std::ofstream &stream, const std::string &name) {
    write<R>(stream, name.size(), uchar_iter(name.begin()), uchar_iter(name.end()));
}

template <record_type R>
void write_int(spdlog::logger &logger, std::ofstream &stream, uint16_t val) {
    uint16_t tmp[1] = {val};
    write<R>(stream, 1, tmp, tmp + 1);
}

template <typename iT>
void write_points(spdlog::logger &logger, std::ofstream &stream, iT begin, iT end) {}

void write_header(spdlog::logger &logger, std::ofstream &stream) {
    write_int<record_type::HEADER>(logger, stream, version);
}

void write_units(spdlog::logger &logger, std::ofstream &stream, double resolution,
                 double user_unit) {
    uint64_t data[2] = {double_to_gds(resolution), double_to_gds(resolution * user_unit)};
    write<record_type::UNITS>(stream, 2, data, data + 2);
}

void write_lib_begin(spdlog::logger &logger, std::ofstream &stream,
                     const std::vector<uint16_t> &time_vec) {
    write_grp_begin<record_type::BGNLIB>(logger, stream, time_vec);
}

void write_lib_name(spdlog::logger &logger, std::ofstream &stream, const std::string &name) {
    write_name<record_type::LIBNAME>(logger, stream, name);
}

void write_lib_end(spdlog::logger &logger, std::ofstream &stream) {
    write_empty<record_type::ENDLIB>(logger, stream);
}

void write_struct_begin(spdlog::logger &logger, std::ofstream &stream,
                        const std::vector<uint16_t> &time_vec) {
    write_grp_begin<record_type::BGNSTR>(logger, stream, time_vec);
}

void write_struct_name(spdlog::logger &logger, std::ofstream &stream, const std::string &name) {
    write_name<record_type::STRNAME>(logger, stream, name);
}

void write_struct_end(spdlog::logger &logger, std::ofstream &stream) {
    write_empty<record_type::ENDSTR>(logger, stream);
}

} // namespace gdsii
} // namespace cbag
