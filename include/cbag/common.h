//
// Created by Eric Chang on 6/28/18.
//


#ifndef CBAG_COMMON_H
#define CBAG_COMMON_H

#include <cstdint>

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
        explicit Transform(coord_t x = 0, coord_t y = 0, Orientation orient = R0)
                :
                x(x), y(y), orient(orient) {}

        coord_t x, y;
        Orientation orient;
    };

    /** A data structure that represents vector indices.
     *
     * The step size of a Range can be either positive or negative, but must not be 0.
     * If start == stop, this Range is considered empty.
     */
    struct Range {
        /** Creates an empty Range.
         */
        Range()
                : start(0), stop(0), step(-1) {}

        /** Create a new vector range.
         *
         * @param start the starting index (inclusive).
         * @param stop  the stopping index (exclusive).
         * @param step  the step size.  Should never be 0, and sign should be consistent with start/stop.
         */
        Range(int32_t start, int32_t stop, int32_t step)
                : start(start), stop(stop), step(step) {}

        /** Returns true if this range contains no index.
         *
         *  @returns true if this range contains no index.
         */
        inline bool empty() { return stop == start; }

        /** Returns number of elements in this Range.
         *
         *  @returns number of elements in this Range.
         */
        inline uint32_t size() { return static_cast<uint32_t>((stop - start) / step); }

        inline bool operator==(const Range &other) const;

        inline bool operator<(const Range &other) const;

        int32_t start, stop, step;
    };

    /** A name unit object representing either a net, a terminal (i.e. pin), or an instance.
     *
     *  This class implements scalar name (like "foo") or vector name (like "bar<3:0>", or "baz<1>") in OpenAccess.
     */
    struct NameUnit {
        /** Creates an empty name unit.
         */
        NameUnit() = default;

        /** Returns true if this name unit has no base name.
         *
         *  @returns true if this name unit has no base name.
         */
        inline bool empty() { return name.empty(); }

        /** Returns true if this name unit is a bus.
         *
         *  @returns true if this name unit is a bus.
         */
        inline bool is_vector() { return !range.empty(); }

        /** Returns number of actual objects (net, terminal, or instance) represented by this name unit.
         *
         *  @returns number of objects represented by this name unit.
         */
        inline uint32_t size() { return (empty()) ? 0 : ((range.empty()) ? 1 : range.size()); }

        bool operator==(const NameUnit &other) const;

        bool operator<(const NameUnit &other) const;

        std::string name;
        Range range;
    };

    /** A name object.
     *
     *  This class implements bundle name in OpenAccess.
     */
    struct Name {
        /** Creates an empty name.
         */
        Name() : unit_list(0) {}

        /** Creates an empty name with vector preallocation.
         */
        explicit Name(unsigned long n) : unit_list(n) {}

        /** Returns true if this name is empty.
         *
         *  @returns true if this name is empty.
         */
        inline bool empty() { return unit_list.empty(); }

        /** Returns true if this name is a bundle.
         *
         *  @returns true if this name is a bundle.
         */
        inline bool is_bundle() { return unit_list.size() > 1; }

        /** Returns number of name units in this name.
         *
         *  @returns number of name units in this name.
         */
        inline uint32_t size() { return static_cast<uint32_t>(unit_list.size()); }

        bool operator<(const Name &other) const;

        std::vector<NameUnit> unit_list;
    };

    /** A class that encodes/decodes Name objects to/from strings.
     *
     *  This class is mainly used to handle different vector or delimiters for different kinds of netlists.
     *  Use a null character for vec_stop if there is no ending vector character.
     */
    class NameFormatter {
    public:
        /** Create a default NameFormatter.
         */
        NameFormatter()
                : delim(','), vec_start('<'), vec_stop('>'), vec_delim(':') {}

        /** Create a new NameFormatter.
         *
         *  @param delim the delimiter character.
         *  @param vec_start character denoting the start of a vector index range.
         *  @param vec_stop character denoting the end of a vector index range.
         *  @param vec_delim the vector index delimiter.
         */
        NameFormatter(char delim, char vec_start, char vec_stop, char vec_delim)
                : delim(delim), vec_start(vec_start), vec_stop(vec_stop), vec_delim(vec_delim) {}

        /** Returns a Name object corresponding to the given string.
         *
         *  @param name_str the string object.
         *  @returns a Name object corresponding to the given string.
         * @throws std::invalid_argument if the given string does not represent a name.
         */
        Name get_name(const std::string &name_str);

        /** Returns a NameUnit object corresponding to the given string.
         *
         *  @param name_str the string object.
         *  @returns a NameUnit object corresponding to the given string.
         * @throws std::invalid_argument if the given string does not represent a name unit.
         */
        NameUnit get_name_unit(const std::string &name_str);

    private:

        /** Returns a Range object corresponding to the given string.
         *
         *  @param range_str the string object.
         *  @returns a Range object corresponding to the given string.
         * @throws std::invalid_argument if the given string does not represent a range.
         */
        Range get_range(const std::string &range_str);

        char delim, vec_start, vec_stop, vec_delim;
    };

    // parameter data structure.
    typedef boost::variant<int32_t, double, std::string> value_t;
    // parameter dictionary.
    typedef std::map<std::string, value_t> ParamMap;

    // Visitor structor for outputing boost variant to YAML
    struct VarYAMLVisitor : public boost::static_visitor<> {

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

        YAML::Emitter *out_ptr;

    };

    // YAML stream out functions.

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const Transform &v) {
        return out << YAML::Flow
                   << YAML::BeginSeq << v.x << v.y << enumToStr(v.orient) << YAML::EndSeq;
    }

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const NameUnit &v) {
        return out << YAML::Flow
                   << YAML::BeginSeq << v.name << v.range.start << v.range.stop << v.range.step << YAML::EndSeq;
    }

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const Name &v) {
        return out << v.unit_list;
    }

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const value_t &v) {

        VarYAMLVisitor visitor(&out);
        boost::apply_visitor(visitor, v);
        return out;
    }
}

#endif //CBAG_COMMON_H
