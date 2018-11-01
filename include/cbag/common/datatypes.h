/** \file datatypes.h
 *  \brief This file defines datatypes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_COMMON_DATATYPES_H
#define CBAG_COMMON_DATATYPES_H

#include <cstdint>
#include <ctime>
#include <string>
#include <variant>

#include <cbag/util/sorted_map.h>

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

/** Type definition for a parameter dictonary.
 */
using param_map = util::sorted_map<std::string, value_t>;

void set_param(param_map &params, std::string &&name,
               const std::variant<int32_t, double, bool, std::string> &val);

} // namespace cbag

#endif // CBAG_COMMON_DATATYPES_H
