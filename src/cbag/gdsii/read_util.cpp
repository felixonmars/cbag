#include <iterator>

#include <fmt/core.h>

#include <cbag/gdsii/read_util.h>
#include <cbag/gdsii/record_type.h>
#include <cbag/gdsii/typedefs.h>
#include <cbag/util/sfinae.h>

namespace cbag {
namespace gdsii {

template <typename T, util::IsInt<T> = 0> T read_bytes(std::ifstream &stream) {
    constexpr auto unit_size = sizeof(T);
    auto ans = static_cast<T>(0);
    for (std::size_t bidx = 0, shft = (unit_size - 1) * 8; bidx < unit_size; ++bidx, shft -= 8) {
        unsigned char tmp = stream.get();
        ans |= tmp << shft;
    }
    return ans;
}

template <record_type R> void read_skip(std::ifstream &stream, std::size_t num_bytes = 0) {
    auto size = static_cast<std::size_t>(read_bytes<uint16_t>(stream)) - 4;
    auto record_val = read_bytes<uint16_t>(stream);
    auto record_expect = static_cast<uint16_t>(R);
    if (record_val != record_expect)
        throw std::runtime_error(
            fmt::format("got gds record {}, expected {}", record_val, record_expect));
    if (num_bytes != 0 && size != num_bytes)
        throw std::runtime_error(
            fmt::format("gds record has size {}, expected {}", size, num_bytes));
    stream.ignore(size);
}

template <record_type R, typename iT, typename ReserveFun>
void read(std::ifstream &stream, iT iter, ReserveFun &&fun, std::size_t num_data = 0) {
    constexpr auto unit_size = sizeof(*iter);
    auto size = static_cast<std::size_t>(read_bytes<uint16_t>(stream)) - 4;
    auto record_val = read_bytes<uint16_t>(stream);
    auto record_expect = static_cast<uint16_t>(R);
    if (record_val != record_expect)
        throw std::runtime_error(
            fmt::format("got gds record {}, expected {}", record_val, record_expect));
    if (num_data == 0) {
        if (size % unit_size != 0)
            throw std::runtime_error(
                fmt::format("gds record size {} not divisible by unit size {}", size, unit_size));
    } else if (size != num_data * unit_size) {
        throw std::runtime_error(
            fmt::format("gds record has size {}, expected {}", size, num_data));
    }

    auto lim = size / unit_size;
    if constexpr (!std::is_same_v<decltype(fun), std::nullptr_t>) {
        fun(lim);
    }

    for (decltype(lim) idx = 0; idx < lim; ++idx, ++iter) {
        *iter = read_bytes<std::decay_t<decltype(*iter)>>(stream);
    }
}

template <record_type R> uint16_t read_int(spdlog::logger &logger, std::ifstream &stream) {
    std::array<uint16_t, 1> ans;
    read<R>(stream, ans.begin(), nullptr, ans.size());
    return ans[0];
}

template <record_type R> std::string read_name(spdlog::logger &logger, std::ifstream &stream) {
    std::string ans;
    read<R>(stream, ans.begin(), [&ans](std::size_t size) { ans.resize(size); });
    return ans;
}

template <record_type R> void read_grp_begin(spdlog::logger &logger, std::ifstream &stream) {
    read_skip<R>(stream, 12 * sizeof(tval_t));
}

void read_header(spdlog::logger &logger, std::ifstream &stream) {
    read_skip<record_type::HEADER>(stream, sizeof(uint16_t));
}

void read_lib_begin(spdlog::logger &logger, std::ifstream &stream) {
    read_grp_begin<record_type::BGNLIB>(logger, stream);
}

std::string read_lib_name(spdlog::logger &logger, std::ifstream &stream) {
    return read_name<record_type::LIBNAME>(logger, stream);
}

void read_units(spdlog::logger &logger, std::ifstream &stream) {
    read_skip<record_type::UNITS>(stream, 2 * sizeof(uint64_t));
}

} // namespace gdsii
} // namespace cbag
