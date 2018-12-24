#ifndef CBAG_COMMON_PARAM_DATA_H
#define CBAG_COMMON_PARAM_DATA_H

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
using value_t = std::variant<int32_t, double, bool, std::string, time_struct, binary_t>;

} // namespace cbag

#endif // CBAG_COMMON_DATATYPES_H
