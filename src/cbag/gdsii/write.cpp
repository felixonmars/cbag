#include <cassert>
#include <cstdint>
#include <vector>

#include <fmt/core.h>

#include <cbag/gdsii/math.h>
#include <cbag/gdsii/write.h>
#include <cbag/util/io.h>

namespace cbag {
namespace gdsii {

void write_header(spdlog::logger &logger, std::ofstream &stream) {
    std::vector<uint16_t> data({version});
    write<uint16_t, record_type::HEADER>(stream, data);
}

void write_units(spdlog::logger &logger, std::ofstream &stream, double resolution,
                 double user_unit) {
    std::vector<uint64_t> data({double_to_gds(resolution), double_to_gds(resolution * user_unit)});
    write<uint64_t, record_type::UNITS>(stream, data);
}

} // namespace gdsii
} // namespace cbag
