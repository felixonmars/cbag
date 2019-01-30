#include <array>
#include <cassert>
#include <cstdint>
#include <vector>

#include <cbag/common/box_t_util.h>
#include <cbag/common/transformation_util.h>
#include <cbag/gdsii/math.h>
#include <cbag/gdsii/record_type.h>
#include <cbag/gdsii/write_util.h>
#include <cbag/layout/polygon.h>
#include <cbag/util/io.h>
#include <cbag/util/sfinae.h>

namespace cbag {
namespace gdsii {

using size_type = uint16_t;

constexpr auto max_size = UINT16_MAX;
constexpr auto type_size = sizeof(record_type);
constexpr auto size_size = sizeof(size_type);
constexpr auto version = static_cast<uint16_t>(5);
constexpr auto text_presentation = static_cast<uint16_t>(0xA000);

template <typename T, typename U> T interpret_as(U val) { return *reinterpret_cast<T *>(&val); }

class uchar_iter {
  private:
    std::string::const_iterator iter;

  public:
    uchar_iter() = default;
    explicit uchar_iter(std::string::const_iterator iter) : iter(std::move(iter)) {}

    unsigned char operator*() { return interpret_as<unsigned char>(*iter); }
    uchar_iter &operator++() {
        ++iter;
        return *this;
    }
    bool operator!=(const uchar_iter &other) { return iter != other.iter; }
};

template <typename iT> class point_xy_iter {
  private:
    iT pt_iter;
    bool is_y = false;

  public:
    point_xy_iter() = default;
    explicit point_xy_iter(iT pt_iter, bool is_y = false)
        : pt_iter(std::move(pt_iter)), is_y(is_y) {}

    uint32_t operator*() const {
        auto val = (is_y) ? pt_iter->y() : pt_iter->x();
        return *reinterpret_cast<uint32_t *>(&val);
    }

    uint32_t x() const { return pt_iter->x(); }

    uint32_t y() const { return pt_iter->y(); }

    point_xy_iter &operator++() {
        if (is_y) {
            ++pt_iter;
        }
        is_y = !is_y;
        return *this;
    }
    bool operator!=(const point_xy_iter &other) const noexcept {
        return pt_iter != other.pt_iter || is_y != other.is_y;
    }
};

template <typename T, util::IsUInt<T> = 0> void write_bytes(std::ostream &stream, T val) {
    constexpr auto unit_size = sizeof(T);
    for (std::size_t bidx = 0, shft = (unit_size - 1) * 8; bidx < unit_size; ++bidx, shft -= 8) {
        auto tmp = static_cast<char>(val >> shft);
        stream.put(tmp);
    }
}

template <record_type R, typename iT>
void write(std::ostream &stream, std::size_t num_data, iT start, iT stop) {
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
void write_grp_begin(spdlog::logger &logger, std::ostream &stream,
                     const std::vector<tval_t> &time_vec) {
    std::vector<tval_t> data(time_vec.begin(), time_vec.end());
    data.insert(data.end(), time_vec.begin(), time_vec.end());
    write<R>(stream, data.size(), data.begin(), data.end());
}

template <record_type R> void write_empty(spdlog::logger &logger, std::ostream &stream) {
    std::array<uint16_t, 0> tmp;
    write<R>(stream, tmp.size(), tmp.begin(), tmp.end());
}

template <record_type R>
void write_name(spdlog::logger &logger, std::ostream &stream, const std::string &name) {
    write<R>(stream, name.size(), uchar_iter(name.begin()), uchar_iter(name.end()));
}

template <record_type R>
void write_int(spdlog::logger &logger, std::ostream &stream, uint16_t val) {
    std::array<uint16_t, 1> tmp{val};
    write<R>(stream, tmp.size(), tmp.begin(), tmp.end());
}

template <typename iT>
void write_points(spdlog::logger &logger, std::ostream &stream, std::size_t num_pts, iT begin,
                  iT end) {
    auto start_iter = point_xy_iter(begin);
    auto xval = start_iter.x();
    auto yval = start_iter.y();
    write<record_type::XY>(stream, 2 * (num_pts + 1), std::move(start_iter), point_xy_iter(end));
    // write first point
    write_bytes(stream, xval);
    write_bytes(stream, yval);
}

std::tuple<uint32_t, uint16_t> get_angle_flag(orientation orient) {
    switch (orient) {
    case oR90:
        return {90, 0x4000};
    case oR180:
        return {180, 0x4000};
    case oR270:
        return {270, 0x4000};
    case oMX:
        return {0, 0x4001};
    case oMXR90:
        return {90, 0x4001};
    case oMY:
        return {180, 0x4001};
    case oMYR90:
        return {270, 0x4001};
    default:
        return {0, 0x4000};
    }
}

void write_transform(spdlog::logger &logger, std::ostream &stream, const transformation &xform,
                     cnt_t nx, cnt_t ny, offset_t spx, offset_t spy) {
    auto [angle, bit_flag] = get_angle_flag(orient(xform));

    write_int<record_type::STRANS>(logger, stream, bit_flag);
    if (angle != 0) {
        std::array<uint64_t, 1> data{double_to_gds((double)angle)};
        write<record_type::ANGLE>(stream, data.size(), data.begin(), data.end());
    }
    if (nx > 1 || ny > 1) {
        // convert BAG array parameters to GDS array parameters
        auto [gds_nx, gds_ny, gds_spx, gds_spy] = cbag::convert_array(xform, nx, ny, spx, spy);
        std::array<uint16_t, 2> nxy{static_cast<uint16_t>(gds_nx), static_cast<uint16_t>(gds_ny)};
        write<record_type::COLROW>(stream, nxy.size(), nxy.begin(), nxy.end());
        auto [x1, y1] = location(xform);
        decltype(spx) x2 = gds_spx * gds_nx;
        decltype(spx) y2 = 0;
        decltype(spx) x3 = 0;
        decltype(spx) y3 = gds_spy * gds_ny;
        xform.transform(x2, y2);
        xform.transform(x3, y3);
        std::array<uint32_t, 6> xy{interpret_as<uint32_t>(x1), interpret_as<uint32_t>(y1),
                                   interpret_as<uint32_t>(x2), interpret_as<uint32_t>(y2),
                                   interpret_as<uint32_t>(x3), interpret_as<uint32_t>(y3)};
        write<record_type::XY>(stream, xy.size(), xy.begin(), xy.end());
    } else {
        std::array<uint32_t, 2> xy{interpret_as<uint32_t>(x(xform)),
                                   interpret_as<uint32_t>(y(xform))};
        write<record_type::XY>(stream, xy.size(), xy.begin(), xy.end());
    }
}

void write_header(spdlog::logger &logger, std::ostream &stream) {
    write_int<record_type::HEADER>(logger, stream, version);
}

void write_units(spdlog::logger &logger, std::ostream &stream, double resolution,
                 double user_unit) {
    std::array<uint64_t, 2> data{double_to_gds(resolution), double_to_gds(resolution * user_unit)};
    write<record_type::UNITS>(stream, data.size(), data.begin(), data.end());
}

void write_lib_begin(spdlog::logger &logger, std::ostream &stream,
                     const std::vector<tval_t> &time_vec) {
    write_grp_begin<record_type::BGNLIB>(logger, stream, time_vec);
}

void write_lib_name(spdlog::logger &logger, std::ostream &stream, const std::string &name) {
    write_name<record_type::LIBNAME>(logger, stream, name);
}

void write_lib_end(spdlog::logger &logger, std::ostream &stream) {
    write_empty<record_type::ENDLIB>(logger, stream);
}

void write_struct_begin(spdlog::logger &logger, std::ostream &stream,
                        const std::vector<tval_t> &time_vec) {
    write_grp_begin<record_type::BGNSTR>(logger, stream, time_vec);
}

void write_struct_name(spdlog::logger &logger, std::ostream &stream, const std::string &name) {
    write_name<record_type::STRNAME>(logger, stream, name);
}

void write_struct_end(spdlog::logger &logger, std::ostream &stream) {
    write_empty<record_type::ENDSTR>(logger, stream);
}

void write_element_end(spdlog::logger &logger, std::ostream &stream) {
    write_empty<record_type::ENDEL>(logger, stream);
}

void write_polygon(spdlog::logger &logger, std::ostream &stream, glay_t layer, gpurp_t purpose,
                   const layout::polygon &poly) {
    write_empty<record_type::BOUNDARY>(logger, stream);
    write_int<record_type::LAYER>(logger, stream, layer);
    write_int<record_type::DATATYPE>(logger, stream, purpose);
    write_points(logger, stream, poly.size(), poly.begin(), poly.end());
    write_element_end(logger, stream);
}

void write_box(spdlog::logger &logger, std::ostream &stream, glay_t layer, gpurp_t purpose,
               const box_t &b) {
    write_empty<record_type::BOX>(logger, stream);
    write_int<record_type::LAYER>(logger, stream, layer);
    write_int<record_type::BOXTYPE>(logger, stream, purpose);

    auto x0 = interpret_as<uint32_t>(xl(b));
    auto x1 = interpret_as<uint32_t>(xh(b));
    auto y0 = interpret_as<uint32_t>(yl(b));
    auto y1 = interpret_as<uint32_t>(yh(b));
    std::array<uint32_t, 10> xy{x0, y0, x1, y0, x1, y1, x0, y1, x0, y0};
    write<record_type::XY>(stream, xy.size(), xy.begin(), xy.end());
    write_element_end(logger, stream);
}

void write_arr_instance(spdlog::logger &logger, std::ostream &stream, const std::string &cell_name,
                        const transformation &xform, cnt_t nx, cnt_t ny, offset_t spx,
                        offset_t spy) {
    write_empty<record_type::AREF>(logger, stream);
    write_name<record_type::SNAME>(logger, stream, cell_name);
    write_transform(logger, stream, xform, nx, ny, spx, spy);
    write_element_end(logger, stream);
}

void write_instance(spdlog::logger &logger, std::ostream &stream, const std::string &cell_name,
                    const transformation &xform, cnt_t nx, cnt_t ny, offset_t spx, offset_t spy) {
    if (nx > 1 || ny > 1) {
        write_arr_instance(logger, stream, cell_name, xform, nx, ny, spx, spy);
    } else {
        write_empty<record_type::SREF>(logger, stream);
        write_name<record_type::SNAME>(logger, stream, cell_name);
        write_transform(logger, stream, xform);
        write_element_end(logger, stream);
    }
}

void write_text(spdlog::logger &logger, std::ostream &stream, glay_t layer, gpurp_t purpose,
                const std::string &text, const transformation &xform) {
    write_empty<record_type::TEXT>(logger, stream);
    write_int<record_type::LAYER>(logger, stream, layer);
    write_int<record_type::TEXTTYPE>(logger, stream, purpose);
    write_int<record_type::PRESENTATION>(logger, stream, text_presentation);
    write_transform(logger, stream, xform);
    write_name<record_type::STRING>(logger, stream, text);
    write_element_end(logger, stream);
}

} // namespace gdsii
} // namespace cbag
