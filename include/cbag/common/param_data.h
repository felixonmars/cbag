#ifndef CBAG_COMMON_PARAM_DATA_H
#define CBAG_COMMON_PARAM_DATA_H

#include <cmath>
#include <cstdint>
#include <ctime>
#include <string>
#include <variant>

namespace cbag {
/** A custom struct representing time data.
 *
 *  This struct is used to distinguish time from int, long, or double, so that
 *  std::variant will not be confused.
 */
struct time_struct {
    time_t time_val;
};

/** A custom struct representing binary data.
 *
 *  This struct is used to distinguish binary from string, so that
 *  std::variant will not be confused.
 */
struct binary_t {
    std::string name;
    std::string bin_val;
};

/** Type definition for a parameter value type.
 */
using value_t = std::variant<int_fast32_t, double_t, bool, std::string, time_struct, binary_t>;

using param_t = std::variant<int_fast32_t, double_t, bool, std::string>;

} // namespace cbag

#endif
