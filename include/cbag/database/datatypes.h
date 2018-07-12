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
    typedef int32_t coord_t;
    typedef int32_t lay_t;
    typedef int32_t purp_t;
    typedef uint32_t dist_t;

    /** Define the orientation enum.
     */
    BETTER_ENUM(Orientation, uint32_t, R0, R90, R180, R270, MY, MYR90, MX, MXR90)

    /** Location and Orientation of an instance.
     */
    struct Transform {
        Transform()
                : x(0), y(0), orient(Orientation::R0) {}

        Transform(coord_t x, coord_t y)
                : x(x), y(y), orient(Orientation::R0) {}

        Transform(coord_t x, coord_t y, Orientation orient)
                : x(x), y(y), orient(orient) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(x);
            ar & BOOST_SERIALIZATION_NVP(y);
            ar & BOOST_SERIALIZATION_NVP(orient);
#pragma clang diagnostic pop
        }

        coord_t x, y;
        Orientation orient;
    };

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

// Serialization routines for enums

BOOST_SERIALIZATION_SPLIT_FREE(cbag::Orientation)

namespace boost {
    namespace serialization {

        // SFINAE trick to make sure this template function only applies if object of class E
        // has _to_string() method.  the auto keyword delay return type declaration to decltype(),
        // decltype() checks if e._to_string() compiles, and if so, this function returns void.
        // if e._to_string() does not compile, this template function is discarded.

        template<class Archive, class E>
        auto save(Archive &ar, const E &e, const unsigned int version)
        -> decltype(e._to_string(), void()) {
            std::string tmp(e._to_string());
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & make_nvp("value", tmp);
#pragma clang diagnostic pop
        }

        // same SFINAE trick to make sure this function only applies to E if
        // E::_from_string() compiles.

        template<class Archive, class E>
        auto load(Archive &ar, E &e, const unsigned int version)
        -> decltype(E::_from_string(), void()) {
            std::string tmp;
            ar >> tmp;
            e = E::_from_string((tmp.c_str()));
        }
    }
}


#endif //CBAG_DATABASE_COMMON_H
