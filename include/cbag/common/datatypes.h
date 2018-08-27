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
#include <map>
#include <string>
#include <variant>

namespace cbag {
/** A custom struct representing time data.
 *
 *  This struct is used to distinguish time from int, long, or double, so that
 *  std::variant will not be confused.
 */
struct Time {
    Time() = default;

    explicit Time(time_t time_val) : time_val(time_val) {}

    time_t time_val;
};

/** A custom struct representing binary data.
 *
 *  This struct is used to distinguish binary from string, so that
 *  std::variant will not be confused.
 */
struct Binary {
    Binary() = default;

    Binary(const char *name, const unsigned char *data, unsigned int size)
        : name(name), bin_val(data, data + size) {}

    std::string name;
    std::string bin_val;
};

/** Type definition for a parameter value type.
 */
typedef std::variant<int32_t, double, bool, std::string, Time, Binary> value_t;

/** Type definition for a parameter dictonary.
 */
typedef std::map<std::string, value_t> ParamMap;

} // namespace cbag

#endif // CBAG_COMMON_DATATYPES_H
