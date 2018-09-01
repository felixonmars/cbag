
#include <cbag/layout/point_t.h>

namespace cbag {
namespace layout {

byte4_t::operator uint32_t() const { return *reinterpret_cast<const uint32_t *>(data); }

byte4_t::operator int32_t() const { return *reinterpret_cast<const int32_t *>(data); }

const byte4_t byte4_t::operator=(int32_t value) {
    *(reinterpret_cast<int32_t *>(data)) = value;
    return *this;
}

const byte4_t byte4_t::operator=(uint32_t value) {
    *(reinterpret_cast<uint32_t *>(data)) = value;
    return *this;
}

byte4_t byte4_t::convert(int32_t value) { return *reinterpret_cast<byte4_t *>(&value); }

point_t::operator uint64_t() const { return *reinterpret_cast<const uint64_t *>(val); }

point_t::operator void *() const { return reinterpret_cast<void *>(operator uint64_t()); }

point_t::operator point_t *() const { return reinterpret_cast<point_t *>(operator uint64_t()); }

point_t::operator const point_t *() const {
    return reinterpret_cast<const point_t *>(operator uint64_t());
}

int32_t point_t::x() const { return val[0].operator int32_t(); }

int32_t point_t::y() const { return val[1].operator int32_t(); }

uint32_t point_t::ux() const { return val[0].operator uint32_t(); }

uint32_t point_t::uy() const { return val[1].operator uint32_t(); }

int32_t point_t::operator[](uint32_t idx) const { return val[idx].operator int32_t(); }

byte4_t &point_t::operator[](uint32_t idx) { return val[idx]; }

const int32_t *point_t::get_val_ptr(uint32_t idx) const {
    return reinterpret_cast<const int32_t *>(val + idx);
}

void point_t::set(uint32_t ux, uint32_t uy) {
    val[0] = ux;
    val[1] = uy;
}

void point_t::set(int32_t x, int32_t y) {
    val[0] = x;
    val[1] = y;
}

void point_t::set(void *ptr) {
    *(reinterpret_cast<uint64_t *>(val)) = reinterpret_cast<uint64_t>(ptr);
}

point_t point_t::create(coord_t x, coord_t y) {
    return {cbag::layout::byte4_t::convert(x), cbag::layout::byte4_t::convert(y)};
}

} // namespace layout
} // namespace cbag
