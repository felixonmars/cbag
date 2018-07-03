//
// Created by Eric Chang on 6/28/18.
//


#ifndef CBAG_COMMON_H
#define CBAG_COMMON_H

#include <cstdint>

#include <boost/preprocessor.hpp>
#include <boost/variant.hpp>
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

DEFINE_ENUM_WITH_STRING_CONVERSIONS(Orientation, (R0)(R90)(R180)(R270)(MY)(MYR90)(MX)(MXR90))

// location and orientation of an instance
struct Transform {
  explicit Transform(coord_t x = 0, coord_t y = 0, Orientation orient = R0) :
      x(x), y(y), orient(orient) {}

  coord_t x, y;
  Orientation orient;
};

// parameter data structure
typedef boost::variant<int32_t, double, std::string> value_t;
typedef std::map<std::string, value_t> ParamMap;

inline YAML::Emitter &operator<<(YAML::Emitter &out, const Transform &v) {
  return out << YAML::Flow
             << YAML::BeginSeq << v.x << v.y << enumToStr(v.orient) << YAML::EndSeq;
}

inline YAML::Emitter &operator<<(YAML::Emitter &out, const value_t &v) {

  struct OutVisitor : public boost::static_visitor<> {

    explicit OutVisitor(YAML::Emitter *out_ptr) : out_ptr(out_ptr) {}

    void operator()(const int32_t &i) const {
      (*out_ptr) << i;
    }

    void operator()(const double &d) const {
      (*out_ptr) << d;
    }

    void operator()(const std::string &s) const {
      (*out_ptr) << YAML::DoubleQuoted << s;
    }

    YAML::Emitter *out_ptr;

  };

  OutVisitor visitor(&out);
  boost::apply_visitor(visitor, v);
  return out;
}
}

#endif //CBAG_COMMON_H
