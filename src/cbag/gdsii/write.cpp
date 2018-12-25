#include <cbag/gdsii/write.h>

namespace cbag {
namespace gdsii {

using size_type = uint16_t;

enum class record_type : uint16_t {
    HEADER = 0x0002,
    BGNLIB = 0x0102,
    LIBNAME = 0x0206,
    UNITS = 0x0305,
};

constexpr auto max_size = UINT16_MAX;
constexpr auto type_size = sizeof(record_type);
constexpr auto size_size = sizeof(size_type);
constexpr auto version = static_cast<int16_t>(5);

template <typename T, typename = util::IsInt<T>>
void write(std::ofstream &stream, record_type rtype, const std::vector<T> &vec) {
    constexpr auto unit_size = sizeof(T);

    auto size_test = unit_size * vec.size() + size_size + type_size;
    if (size_test > max_size)
        throw std::runtime_error("Maximum GDS record size exceeded.");

    auto size = static_cast<size_type>(size_test);
    stream.write(reinterpret_cast<char *>(&size), size_size);
    stream.write(reinterpret_cast<char *>(&rtype), type_size);
    stream.write(reinterpret_cast<const char *>(vec.data()), sizeof(T) * vec.size());
}

void write_header(std::ofstream &stream) {
    std::vector<int16_t> data({version});
    write<int16_t>(stream, record_type::HEADER, data);
}

} // namespace gdsii
} // namespace cbag
