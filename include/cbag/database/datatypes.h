/** \file datatypes.h
 *  \brief This file defines datatypes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */


#ifndef CBAG_DATABASE_COMMON_H
#define CBAG_DATABASE_COMMON_H

#include <iostream>
#include <cstdint>

#include <boost/variant.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/variant.hpp>

#include <enum.h>

namespace cbag {
    /** A custom struct representing time data.
     *
     *  This struct is used to distinguish time from int, long, or double, so that boost::variant
     *  will not be confused.
     */
    struct Time {
        Time() = default;

        explicit Time(time_t time_val) : time_val(time_val) {}

        // standard out printing
        friend std::ostream &operator<<(std::ostream &os, const Time &t) {
            return os << "[time, " << t.time_val << "]";
        }

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(time_val);
#pragma clang diagnostic pop
        }

        time_t time_val;
    };

    /** A custom struct representing binary data.
     *
     *  This struct is used to distinguish binary from string, so that boost::variant
     *  will not be confused.
     */
    struct Binary {
        Binary() = default;

        Binary(const unsigned char *data, unsigned int size) : bin_val(data, data + size) {}

        // standard out printing
        friend std::ostream &operator<<(std::ostream &os, const Binary &s) {
            return os << "[bin, " << s.bin_val << "]";
        }

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(bin_val);
#pragma clang diagnostic pop
        }

        std::string bin_val;
    };

    /** Type definition for a parameter value type.
     */
    typedef boost::variant<int32_t, double, std::string, Time, Binary> value_t;

    /** Type definition for a parameter dictonary.
     */
    typedef std::map<std::string, value_t> ParamMap;

}

#endif //CBAG_DATABASE_COMMON_H
