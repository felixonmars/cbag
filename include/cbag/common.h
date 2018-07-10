//
// Created by Eric Chang on 6/28/18.
//


#ifndef CBAG_COMMON_H
#define CBAG_COMMON_H

#include <utility>

#include <boost/preprocessor.hpp>
#include <boost/variant.hpp>
#include <boost/tokenizer.hpp>
#include <yaml-cpp/yaml.h>


// Macro that allows easy conversion between enum and strings
// https://stackoverflow.com/questions/5093460/how-to-convert-an-enum-type-variable-to-a-string
#define X_DEFINE_ENUM_WITH_STRING_CONVERSIONS_TOSTRING_CASE(r, data, elem)    \
    case elem : return BOOST_PP_STRINGIZE(elem);

#define DEFINE_ENUM_WITH_STRING_CONVERSIONS(name, enumerators)                \
    enum name {                                                               \
        BOOST_PP_SEQ_ENUM(enumerators)                                        \
    };                                                                        \
                                                                              \
    inline const char* enumToStr(name v)                                      \
    {                                                                         \
        switch (v)                                                            \
        {                                                                     \
            BOOST_PP_SEQ_FOR_EACH(                                            \
                X_DEFINE_ENUM_WITH_STRING_CONVERSIONS_TOSTRING_CASE,          \
                name,                                                         \
                enumerators                                                   \
            )                                                                 \
            default: return "[Unknown " BOOST_PP_STRINGIZE(name) "]";         \
        }                                                                     \
    }

namespace cbag {
    typedef int32_t coord_t;
    typedef int32_t lay_t;
    typedef int32_t purp_t;

    /** Define the orientation enum.
     */
    DEFINE_ENUM_WITH_STRING_CONVERSIONS(Orientation, (R0)(R90)(R180)(R270)(MY)(MYR90)(MX)(MXR90))

    /** Location and Orientation of an instance.
     */
    struct Transform {
        Transform()
                : x(0), y(0), orient(R0) {}

        Transform(coord_t x, coord_t y)
                : x(x), y(y), orient(R0) {}

        Transform(coord_t x, coord_t y, Orientation orient)
                : x(x), y(y), orient(orient) {}

        coord_t x, y;
        Orientation orient;
    };

    // A custom struct representing time.
    // This struct is used so that boost variant will not confuse time with int or double.
    struct Time {

        Time() = default;

        explicit Time(time_t time_val) : time_val(time_val) {}

        time_t time_val;
    };
    // parameter data structure.
    typedef boost::variant<int32_t, double, std::string, Time, std::vector<unsigned char> > value_t;
    // parameter dictionary.
    typedef std::map<std::string, value_t> ParamMap;

    // YAML stream out functions.

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const Transform &v) {
        return out << YAML::Flow
                   << YAML::BeginSeq << v.x << v.y << enumToStr(v.orient) << YAML::EndSeq;
    }

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const Time &v) {
        return out << YAML::Flow << YAML::BeginSeq << "time" << v.time_val << YAML::EndSeq;
    }

}

// define YAML overload for cbag::value_t in boost namespace, because cbag::value_t is a typedef of a boost
// data structure.
namespace boost {
    // Visitor structor for outputing boost variant to YAML
    struct VarYAMLVisitor : public static_visitor<> {

        explicit VarYAMLVisitor(YAML::Emitter *out_ptr)
                : out_ptr(out_ptr) {}

        void operator()(const int32_t &i) const {
            (*out_ptr) << i;
        }

        void operator()(const double &d) const {
            (*out_ptr) << d;
        }

        void operator()(const std::string &s) const {
            (*out_ptr) << YAML::DoubleQuoted << s;
        }

        void operator()(const cbag::Time &t) const {
            (*out_ptr) << t;
        }

        void operator()(const std::vector<unsigned char> &v) const {
            (*out_ptr) << YAML::Binary(v.data(), v.size());
        }

        YAML::Emitter *out_ptr;

    };

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const cbag::value_t &v) {

        VarYAMLVisitor visitor(&out);
        boost::apply_visitor(visitor, v);
        return out;
    }
}

#endif //CBAG_COMMON_H
