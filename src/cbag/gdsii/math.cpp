#include <cassert>
#include <cmath>
#include <ctime>

#include <cbag/gdsii/math.h>

namespace cbag {
namespace gdsii {

// from IEEE double standard
// I did not use cfloat macro because it counts a hidden bit.
constexpr auto dbl_mantissa = 52;
constexpr auto gds_mantissa = 56;
constexpr auto gds_exp = 7;
constexpr auto gds_exp_max = ((1 << gds_exp) - 1) << 2;
constexpr auto one_64b = static_cast<uint64_t>(1);

uint64_t convert_double(double val) {
    int exp_dbl;
    auto frac = std::frexp(val, &exp_dbl);

    // at this point, 0.5 <= |frac| < 1, |frac| * 2^(exp) = |val|

    // get sign bit
    auto frac_bits = *reinterpret_cast<uint64_t *>(&frac);
    auto sgn = frac_bits & (one_64b << 63);

    // get mantissa bits (adding implicit 1), and convert number of
    // bits to gds_mantissa
    constexpr auto tmp_flag = one_64b << dbl_mantissa;
    constexpr auto delta = gds_mantissa - (dbl_mantissa + 1);
    auto mantissa = tmp_flag | (frac_bits & (tmp_flag - 1));

    if constexpr (delta > 0) {
        mantissa <<= delta;
    } else if constexpr (delta < 0) {
        mantissa >>= delta;
    }

    // convert exponent (need to be multiple of 4)
    auto shift_extra = 4 - (exp_dbl & (4 - 1));
    mantissa >>= shift_extra;

    exp_dbl += (256 + shift_extra);
    assert(0 <= exp_dbl && exp_dbl <= gds_exp_max);
    auto exp = (((uint64_t)exp_dbl) >> 2) << gds_mantissa;

    return sgn | exp | mantissa;
}

std::vector<uint16_t> get_gds_time() {
    auto ep_time = std::time(nullptr);
    auto loc_time = std::localtime(&ep_time);
    return {
        static_cast<uint16_t>(loc_time->tm_year), static_cast<uint16_t>(loc_time->tm_mon + 1),
        static_cast<uint16_t>(loc_time->tm_mday), static_cast<uint16_t>(loc_time->tm_hour),
        static_cast<uint16_t>(loc_time->tm_min),  static_cast<uint16_t>(loc_time->tm_sec),
    };
}

} // namespace gdsii
} // namespace cbag
