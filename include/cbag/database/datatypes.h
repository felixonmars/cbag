/** \file datatypes.h
 *  \brief This file defines datatypes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */


#ifndef CBAG_DATABASE_COMMON_H
#define CBAG_DATABASE_COMMON_H

#include <cstdint>

#include <boost/variant.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>


namespace cbag {
    /** A custom struct representing time data.
     *
     *  This struct is used to distinguish time from int, long, or double, so that boost::variant
     *  will not be confused.
     */
    struct Time {
        Time() = default;

        explicit Time(time_t time_val) : time_val(time_val) {}

        time_t time_val;
    };

    /** A custom struct representing binary data.
     *
     *  This struct is used to distinguish binary from string, so that boost::variant
     *  will not be confused.
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
    typedef boost::variant<int32_t, double, std::string, Time, Binary> value_t;

    /** Type definition for a parameter dictonary.
     */
    typedef std::map<std::string, value_t> ParamMap;

}

BOOST_FUSION_ADAPT_STRUCT(cbag::Time,
        time_val
)

BOOST_FUSION_ADAPT_STRUCT(cbag::Binary,
                          name, bin_val
)

#endif //CBAG_DATABASE_COMMON_H
