#include <iterator>

#include <fmt/core.h>

#include <cbag/common/transformation_util.h>
#include <cbag/gdsii/read_util.h>
#include <cbag/gdsii/typedefs.h>
#include <cbag/layout/instance.h>
#include <cbag/layout/label.h>
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

std::tuple<record_type, std::size_t> read_record_header(std::ifstream &stream) {
    auto size = static_cast<std::size_t>(read_bytes<uint16_t>(stream)) - 4;
    auto record_val = read_bytes<uint16_t>(stream);

    return {static_cast<record_type>(record_val), size};
}

template <record_type R, std::size_t unit_size = 1, std::size_t num_data = 0>
std::size_t check_record_header(std::ifstream &stream) {
    auto [actual, size] = read_record_header(stream);
    if (actual != R)
        throw std::runtime_error(fmt::format("got gds record {}, expected {}",
                                             static_cast<int>(actual), static_cast<int>(R)));

    auto ans = size / unit_size;
    auto mod = size % unit_size;
    if (mod != 0)
        throw std::runtime_error(
            fmt::format("gds record size {} not divisible by unit size {}", size, unit_size));

    if (num_data != 0 && ans != num_data) {
        throw std::runtime_error(
            fmt::format("gds record has {} elements, expected {}", ans, num_data));
    }
    return ans;
}

template <record_type R, std::size_t unit_size = 1, std::size_t num_data = 0>
void read_skip(std::ifstream &stream) {
    auto num = check_record_header<R, unit_size, num_data>(stream);
    stream.ignore(num * unit_size);
}

template <record_type R> std::string read_name(spdlog::logger &logger, std::ifstream &stream) {
    std::string ans;
    auto num = check_record_header<R, 1>(stream);
    ans.reserve(num);
    for (decltype(num) idx = 0; idx < num; ++idx) {
        ans.push_back(read_bytes<char>(stream));
    }
    return ans;
}

template <record_type R> void read_grp_begin(spdlog::logger &logger, std::ifstream &stream) {
    read_skip<R, sizeof(tval_t), 12>(stream);
}

void read_header(spdlog::logger &logger, std::ifstream &stream) {
    read_skip<record_type::HEADER, sizeof(uint16_t), 1>(stream);
}

void read_lib_begin(spdlog::logger &logger, std::ifstream &stream) {
    read_grp_begin<record_type::BGNLIB>(logger, stream);
}

std::string read_lib_name(spdlog::logger &logger, std::ifstream &stream) {
    return read_name<record_type::LIBNAME>(logger, stream);
}

void read_units(spdlog::logger &logger, std::ifstream &stream) {
    read_skip<record_type::UNITS, sizeof(uint64_t), 2>(stream);
}

std::string read_struct_name(spdlog::logger &logger, std::ifstream &stream) {
    // TODO: implement this
    return "";
}

std::tuple<gds_layer_t, transformation, std::string>
read_text(spdlog::logger &logger, std::ifstream &stream, std::size_t size) {
    // TODO: implement this
    return {gds_layer_t{0, 0}, make_xform(), ""};
}

layout::instance read_instance(spdlog::logger &logger, std::ifstream &stream, std::size_t size) {
    // TODO: implement this
    return {};
}

layout::instance read_arr_instance(spdlog::logger &logger, std::ifstream &stream,
                                   std::size_t size) {
    // TODO: implement this
    return {};
}

std::tuple<gds_layer_t, box_t> read_box(spdlog::logger &logger, std::ifstream &stream,
                                        std::size_t size) {
    // TODO: implement this
    return {gds_layer_t{0, 0}, box_t{}};
}

std::tuple<gds_layer_t, layout::polygon> read_boundary(spdlog::logger &logger,
                                                       std::ifstream &stream, std::size_t size) {
    // TODO: implement this
    return {gds_layer_t{0, 0}, layout::polygon{}};
}

} // namespace gdsii
} // namespace cbag
