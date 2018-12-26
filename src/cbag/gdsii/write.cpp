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

void write_begin(spdlog::logger &logger, std::ofstream &stream, record_type rec_type,
                 const std::vector<uint16_t> &time_vec) {
    std::vector<uint16_t> data(time_vec.begin(), time_vec.end());
    data.insert(data.end(), time_vec.begin(), time_vec.end());
    write(stream, rec_type, data);
}

void write_name(spdlog::logger &logger, std::ofstream &stream, record_type rec_type,
                const std::string &name) {
    std::vector<unsigned char> data(name.begin(), name.end());
    write(stream, rec_type, data);
}

void write_units(spdlog::logger &logger, std::ofstream &stream, double resolution,
                 double user_unit) {
    std::vector<uint64_t> data({double_to_gds(resolution), double_to_gds(resolution * user_unit)});
    write(stream, record_type::UNITS, data);
}

void write_end(spdlog::logger &logger, std::ofstream &stream, record_type rec_type) {
    std::vector<uint16_t> data;
    write(stream, rec_type, data);
}

} // namespace gdsii
} // namespace cbag
