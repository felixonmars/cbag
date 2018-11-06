
#ifndef CBAG_COMMON_SIG_TYPE_H
#define CBAG_COMMON_SIG_TYPE_H

#include <cstdint>

namespace cbag {

enum class sig_type : uint32_t {
    signal,
    power,
    ground,
    clock,
    tieOff,
    tieHi,
    tieLo,
    analog,
    scan,
    reset
};
constexpr sig_type stSignal = sig_type::signal;
constexpr sig_type stPower = sig_type::power;
constexpr sig_type stGround = sig_type::ground;
constexpr sig_type stClock = sig_type::clock;
constexpr sig_type stTieoff = sig_type::tieOff;
constexpr sig_type stTieHi = sig_type::tieHi;
constexpr sig_type stTieLo = sig_type::tieLo;
constexpr sig_type stAnalog = sig_type::analog;
constexpr sig_type stScan = sig_type::scan;
constexpr sig_type stReset = sig_type::reset;

} // namespace cbag

#endif
