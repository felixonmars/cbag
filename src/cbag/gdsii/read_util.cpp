#include <iterator>

#include <fmt/core.h>

#include <cbag/common/point.h>
#include <cbag/common/transformation_util.h>
#include <cbag/gdsii/math.h>
#include <cbag/gdsii/read_util.h>
#include <cbag/gdsii/typedefs.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/instance.h>
#include <cbag/layout/label.h>
#include <cbag/layout/pt_traits.h>

namespace cbag {
namespace gdsii {

std::tuple<record_type, std::size_t> read_record_header(std::istream &stream) {
    auto size = static_cast<std::size_t>(read_bytes<uint16_t>(stream)) - 4;
    auto record_val = read_bytes<uint16_t>(stream);

    return {static_cast<record_type>(record_val), size};
}

std::tuple<record_type, std::size_t> print_record_header(std::istream &stream) {
    auto ans = read_record_header(stream);
    std::cout << to_string(std::get<0>(ans)) << std::endl;
    return ans;
}

template <record_type R, std::size_t unit_size = 1, std::size_t num_data = 0>
std::size_t check_record_header(std::istream &stream) {
    auto [actual, size] = read_record_header(stream);
    if (actual != R)
        throw std::runtime_error(fmt::format("got gds record {:#x}, expected {:#x}",
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
void read_skip(std::istream &stream) {
    auto num = check_record_header<R, unit_size, num_data>(stream);
    stream.ignore(num * unit_size);
}

template <record_type R> uint16_t read_int(spdlog::logger &logger, std::istream &stream) {
    check_record_header<R, sizeof(uint16_t), 1>(stream);
    return read_bytes<uint16_t>(stream);
}

std::tuple<uint16_t, uint16_t> read_col_row(spdlog::logger &logger, std::istream &stream) {
    check_record_header<record_type::COLROW, sizeof(uint16_t), 2>(stream);
    auto nx = read_bytes<uint16_t>(stream);
    auto ny = read_bytes<uint16_t>(stream);
    return {nx, ny};
}

template <record_type R> double read_double(spdlog::logger &logger, std::istream &stream) {
    check_record_header<R, sizeof(uint64_t), 1>(stream);
    return gds_to_double(read_bytes<uint64_t>(stream));
}

template <record_type R> std::string read_name(spdlog::logger &logger, std::istream &stream) {
    std::string ans;
    auto num = check_record_header<R, sizeof(char)>(stream);
    ans.reserve(num);
    for (decltype(num) idx = 0; idx < num; ++idx) {
        ans.push_back(read_bytes<char>(stream));
    }
    return ans;
}

template <record_type R> void read_grp_begin(spdlog::logger &logger, std::istream &stream) {
    read_skip<R, sizeof(tval_t), 12>(stream);
}

void read_header(spdlog::logger &logger, std::istream &stream) {
    read_skip<record_type::HEADER, sizeof(uint16_t), 1>(stream);
}

void read_lib_begin(spdlog::logger &logger, std::istream &stream) {
    read_grp_begin<record_type::BGNLIB>(logger, stream);
}

std::string read_lib_name(spdlog::logger &logger, std::istream &stream) {
    return read_name<record_type::LIBNAME>(logger, stream);
}

void read_units(spdlog::logger &logger, std::istream &stream) {
    read_skip<record_type::UNITS, sizeof(uint64_t), 2>(stream);
}

std::string read_struct_name(spdlog::logger &logger, std::istream &stream) {
    return read_name<record_type::STRNAME>(logger, stream);
}

void read_ele_end(spdlog::logger &logger, std::istream &stream) {
    check_record_header<record_type::ENDEL, sizeof(uint16_t), 0>(stream);
}

transformation read_transform_angle(spdlog::logger &logger, std::istream &stream) {
    auto bit_flag = read_int<record_type::STRANS>(logger, stream);

    if ((bit_flag & (1 << 13)) != 0) {
        throw std::runtime_error("GDS Magnification in transform is not supported.");
    }

    auto ans = make_xform();
    if ((bit_flag & (1 << 15)) != 0) {
        set_orient(ans, oMX);
    }

    if ((bit_flag & (1 << 14)) != 0) {
        auto ang_dbl = read_double<record_type::ANGLE>(logger, stream);
        auto angle = static_cast<int>(ang_dbl);
        switch (angle) {
        case 0:
            break;
        case 90:
            transform_by(ans, make_xform(0, 0, oR90));
            break;
        case 180:
            transform_by(ans, make_xform(0, 0, oR180));
            break;
        case 270:
            transform_by(ans, make_xform(0, 0, oR270));
            break;
        default:
            throw std::runtime_error("GDS rotation angle not supported: " + std::to_string(angle));
        }
    }

    return ans;
}

point read_point(std::istream &stream) {
    auto x = read_bytes<int32_t>(stream);
    auto y = read_bytes<int32_t>(stream);
    return {x, y};
}

transformation read_transform(spdlog::logger &logger, std::istream &stream) {
    auto ans = read_transform_angle(logger, stream);
    check_record_header<record_type::XY, sizeof(int32_t), 2>(stream);
    auto [x, y] = read_point(stream);
    move_by(ans, x, y);

    return ans;
}

std::tuple<gds_layer_t, transformation, std::string> read_text(spdlog::logger &logger,
                                                               std::istream &stream) {
    auto glay = read_int<record_type::LAYER>(logger, stream);
    auto gpurp = read_int<record_type::TEXTTYPE>(logger, stream);
    read_skip<record_type::PRESENTATION, sizeof(uint16_t), 1>(stream);
    auto xform = read_transform(logger, stream);

    auto text = read_name<record_type::STRING>(logger, stream);
    read_ele_end(logger, stream);

    return {gds_layer_t{glay, gpurp}, std::move(xform), std::move(text)};
}

std::tuple<gds_layer_t, layout::polygon> read_box(spdlog::logger &logger, std::istream &stream) {
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
                                                       std::istream &stream) {
    auto glay = read_int<record_type::LAYER>(logger, stream);
    auto gpurp = read_int<record_type::DATATYPE>(logger, stream);
    // divide by 2 to get number of points instead of number of coordinates.
    auto num = check_record_header<record_type::XY, sizeof(int32_t)>(stream) / 2;

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

std::string read_inst_name(spdlog::logger &logger, std::istream &stream, std::size_t &cnt) {
    auto [rtype, rsize] = read_record_header(stream);
    std::string inst_name;
    switch (rtype) {
    case record_type::ENDEL:
        inst_name = "X" + std::to_string(cnt);
        ++cnt;
        break;
    case record_type::PROPATTR: {
        auto prop_code = read_bytes<uint16_t>(stream);
        if (prop_code != PROP_INST_NAME) {
            throw std::runtime_error(
                fmt::format("Unexpected gds property code {} for instance.", prop_code));
        }
        inst_name = read_name<record_type::PROPVALUE>(logger, stream);
        read_ele_end(logger, stream);
        break;
    }
    default:
        throw std::runtime_error(fmt::format("Unexpected gds record type {} at end of instance.",
                                             static_cast<int>(rtype)));
    }
    return inst_name;
}

layout::instance
read_instance(spdlog::logger &logger, std::istream &stream, std::size_t &cnt,
              const std::unordered_map<std::string, layout::cellview *> &master_map) {
    auto cell_name = read_name<record_type::SNAME>(logger, stream);

    auto iter = master_map.find(cell_name);
    if (iter == master_map.end()) {
        auto msg = fmt::format("Cannot find layout cellview {} in GDS file.", cell_name);
        logger.error(msg);
        throw std::runtime_error(msg);
    }
    auto master = iter->second;
    auto xform = read_transform(logger, stream);
    auto inst_name = read_inst_name(logger, stream, cnt);
    return {std::move(inst_name), master, std::move(xform)};
}

layout::instance
read_arr_instance(spdlog::logger &logger, std::istream &stream, std::size_t &cnt,
                  const std::unordered_map<std::string, layout::cellview *> &master_map) {
    auto cell_name = read_name<record_type::SNAME>(logger, stream);

    auto iter = master_map.find(cell_name);
    if (iter == master_map.end())
        throw std::runtime_error(
            fmt::format("Cannot find layout cellview {} in GDS file.", cell_name));
    auto master = iter->second;

    auto xform = read_transform_angle(logger, stream);

    auto [gds_nx, gds_ny] = read_col_row(logger, stream);
    check_record_header<record_type::XY, sizeof(int32_t), 6>(stream);
    std::array<point, 3> pt_vec;
    pt_vec[0] = read_point(stream);
    pt_vec[1] = read_point(stream);
    pt_vec[2] = read_point(stream);
    auto inst_name = read_inst_name(logger, stream, cnt);

    move_by(xform, pt_vec[0][0], pt_vec[0][1]);
    auto gds_spx = pt_vec[1][0] / gds_nx;
    auto gds_spy = pt_vec[2][1] / gds_ny;

    auto [nx, ny, spx, spy] = cbag::convert_gds_array(xform, gds_nx, gds_ny, gds_spx, gds_spy);

    return {std::move(inst_name), master, std::move(xform), nx, ny, spx, spy};
}

void print_time(std::istream &stream) {
    struct tm val;
    auto timeinfo = &val;
    timeinfo->tm_year = read_bytes<uint16_t>(stream);
    timeinfo->tm_mon = read_bytes<uint16_t>(stream) - 1;
    timeinfo->tm_mday = read_bytes<uint16_t>(stream);
    timeinfo->tm_hour = read_bytes<uint16_t>(stream);
    timeinfo->tm_min = read_bytes<uint16_t>(stream);
    timeinfo->tm_sec = read_bytes<uint16_t>(stream);

    auto time_obj = std::mktime(timeinfo);
    std::cout << std::asctime(std::localtime(&time_obj));
}

bool print_record(std::istream &stream) {
    auto [record, size] = print_record_header(stream);
    auto ans = false;
    switch (record) {
    // empty records
    case record_type::ENDLIB:
        ans = true;
    case record_type::ENDSTR:
    case record_type::BOUNDARY:
    case record_type::SREF:
    case record_type::AREF:
    case record_type::TEXT:
    case record_type::ENDEL:
    case record_type::BOX:
        break;
    // bit flag records
    case record_type::PRESENTATION:
    case record_type::STRANS:
        std::cout << fmt::format("bit flag: {:#06x}", read_bytes<uint16_t>(stream)) << std::endl;
        break;
    case record_type::HEADER:
    case record_type::LAYER:
    case record_type::DATATYPE:
    case record_type::TEXTTYPE:
    case record_type::PROPATTR:
    case record_type::BOXTYPE:
        std::cout << fmt::format("value: {}", read_bytes<uint16_t>(stream)) << std::endl;
        break;
    case record_type::BGNLIB:
    case record_type::BGNSTR:
        std::cout << "modification time: " << std::endl;
        print_time(stream);
        std::cout << "access time: " << std::endl;
        print_time(stream);
        break;
    case record_type::COLROW: {
        auto nx = read_bytes<uint16_t>(stream);
        auto ny = read_bytes<uint16_t>(stream);
        std::cout << fmt::format("nx: {}, ny: {}", nx, ny) << std::endl;
        break;
    }

    case record_type::WIDTH:
        std::cout << fmt::format("value: {}", read_bytes<int32_t>(stream)) << std::endl;
        break;
    case record_type::XY: {
        auto num_pts = size / 4 / 2;
        for (std::size_t idx = 0; idx < num_pts; ++idx) {
            auto x = read_bytes<int32_t>(stream);
            auto y = read_bytes<int32_t>(stream);
            std::cout << fmt::format("xy: ({}, {})", x, y) << std::endl;
        }
        break;
    }

    case record_type::UNITS:
        std::cout << fmt::format("resolution: {}", gds_to_double(read_bytes<uint64_t>(stream)))
                  << std::endl;
        std::cout << fmt::format("res unit: {}", gds_to_double(read_bytes<uint64_t>(stream)))
                  << std::endl;
        break;
    case record_type::MAG:
    case record_type::ANGLE:
        std::cout << fmt::format("value: {}", gds_to_double(read_bytes<uint64_t>(stream)))
                  << std::endl;
        break;
    case record_type::LIBNAME:
    case record_type::STRNAME:
    case record_type::SNAME:
    case record_type::STRING:
    case record_type::PROPVALUE: {
        std::string ans;
        ans.reserve(size);
        for (std::size_t idx = 0; idx < size; ++idx) {
            ans.push_back(read_bytes<char>(stream));
        }
        std::cout << "value: " << ans << std::endl;
        break;
    }
    default:
        throw std::invalid_argument(
            fmt::format("Unknown record type: {:#06x}", static_cast<uint16_t>(record)));
    }

    return ans;
}

} // namespace gdsii
} // namespace cbag
