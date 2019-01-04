#include <iterator>

#include <fmt/core.h>

#include <cbag/common/point.h>
#include <cbag/common/transformation_util.h>
#include <cbag/gdsii/math.h>
#include <cbag/gdsii/read_util.h>
#include <cbag/gdsii/typedefs.h>
#include <cbag/layout/instance.h>
#include <cbag/layout/label.h>
#include <cbag/layout/pt_traits.h>
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

template <record_type R> uint16_t read_int(spdlog::logger &logger, std::ifstream &stream) {
    check_record_header<R, sizeof(uint16_t), 1>(stream);
    return read_bytes<uint16_t>(stream);
}

template <record_type R> double read_double(spdlog::logger &logger, std::ifstream &stream) {
    check_record_header<R, sizeof(uint64_t), 1>(stream);
    return gds_to_double(read_bytes<uint64_t>(stream));
}

template <record_type R> std::string read_name(spdlog::logger &logger, std::ifstream &stream) {
    std::string ans;
    auto num = check_record_header<R, sizeof(char)>(stream);
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
    return read_name<record_type::STRNAME>(logger, stream);
}

void read_ele_end(spdlog::logger &logger, std::ifstream &stream) {
    check_record_header<record_type::ENDEL, sizeof(uint16_t), 0>(stream);
}

transformation read_transform(spdlog::logger &logger, std::ifstream &stream) {
    auto bit_flag = read_int<record_type::STRANS>(logger, stream);

    if ((bit_flag & (1 << 13)) != 0) {
        throw std::runtime_error("GDS Magnification in transform is not supported.");
    }

    transformation ans;
    if ((bit_flag & 1) != 0) {
        set_orient(ans, oMX);
    }

    if ((bit_flag & (1 << 14)) != 0) {
        auto ang_dbl = read_double<record_type::ANGLE>(logger, stream);
        auto angle = static_cast<int>(ang_dbl);
        switch (angle) {
        case 0:
            break;
        case 90:
            ans += make_xform(0, 0, oR90);
            break;
        case 180:
            ans += make_xform(0, 0, oR180);
            break;
        case 270:
            ans += make_xform(0, 0, oR270);
            break;
        default:
            throw std::runtime_error("GDS rotation angle not supported: " + std::to_string(angle));
        }
    }

    return ans;
}

point read_point(std::ifstream &stream) {
    auto x = read_bytes<int32_t>(stream);
    auto y = read_bytes<int32_t>(stream);
    return {x, y};
}

std::tuple<gds_layer_t, transformation, std::string> read_text(spdlog::logger &logger,
                                                               std::ifstream &stream) {
    auto glay = read_int<record_type::LAYER>(logger, stream);
    auto gpurp = read_int<record_type::TEXTTYPE>(logger, stream);
    read_skip<record_type::PRESENTATION, sizeof(uint16_t), 1>(stream);
    auto xform = read_transform(logger, stream);

    check_record_header<record_type::XY, sizeof(int32_t), 2>(stream);
    auto [x, y] = read_point(stream);
    move_by(xform, x, y);

    auto text = read_name<record_type::STRING>(logger, stream);
    read_ele_end(logger, stream);

    return {gds_layer_t{glay, gpurp}, std::move(xform), std::move(text)};
}

std::tuple<gds_layer_t, layout::polygon> read_box(spdlog::logger &logger, std::ifstream &stream,
                                                  std::size_t size) {
    auto glay = read_int<record_type::LAYER>(logger, stream);
    auto gpurp = read_int<record_type::BOXTYPE>(logger, stream);
    check_record_header<record_type::XY, sizeof(int32_t), 10>(stream);

    std::array<point, 5> pt_vec;
    pt_vec[0] = read_point(stream);
    pt_vec[1] = read_point(stream);
    pt_vec[2] = read_point(stream);
    pt_vec[3] = read_point(stream);
    pt_vec[4] = read_point(stream);
    read_ele_end(logger, stream);

    layout::polygon poly;
    poly.set(pt_vec.begin(), pt_vec.end());

    return {gds_layer_t{glay, gpurp}, std::move(poly)};
}

std::tuple<gds_layer_t, layout::polygon> read_boundary(spdlog::logger &logger,
                                                       std::ifstream &stream, std::size_t size) {
    auto glay = read_int<record_type::LAYER>(logger, stream);
    auto gpurp = read_int<record_type::DATATYPE>(logger, stream);
    auto num = check_record_header<record_type::XY, sizeof(int32_t)>(stream);

    std::vector<point> pt_vec;
    pt_vec.reserve(num);
    for (decltype(num) idx = 0; idx < num; ++idx) {
        pt_vec.push_back(read_point(stream));
    }
    read_ele_end(logger, stream);

    layout::polygon poly;
    poly.set(pt_vec.begin(), pt_vec.end());

    return {gds_layer_t{glay, gpurp}, std::move(poly)};
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

} // namespace gdsii
} // namespace cbag
