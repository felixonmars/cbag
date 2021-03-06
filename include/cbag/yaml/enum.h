/** \file yaml_primitives.h
 *  \brief This file declares YAML serialization methods for primitive objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_ENUM_H
#define CBAG_YAML_ENUM_H

#include <yaml-cpp/yaml.h>

#include <cbag/enum/design_output.h>
#include <cbag/enum/font_t.h>
#include <cbag/enum/orientation.h>
#include <cbag/enum/path_style.h>
#include <cbag/enum/sig_type.h>
#include <cbag/enum/term_attr_type.h>
#include <cbag/enum/term_type.h>
#include <cbag/enum/text_align.h>
#include <cbag/enum/text_disp_format.h>

namespace YAML {

template <typename T> using IsEnum = std::enable_if_t<std::is_enum_v<T>, int>;

template <typename T, IsEnum<T> = 0> struct convert_enum {
    static Node encode(const T &rhs) {
        Node n;
        n = static_cast<int>(rhs);
        return n;
    }

    static bool decode(const Node &node, T &rhs) {
        if (node.IsScalar()) {
            try {
                rhs = static_cast<T>(node.as<int>());
                return true;
            } catch (...) {
                return false;
            }
        } else {
            return false;
        }
    }
};

template <> struct convert<cbag::design_output> : public convert_enum<cbag::design_output> {};

template <> struct convert<cbag::orientation> : public convert_enum<cbag::orientation> {};

template <> struct convert<cbag::path_style> : public convert_enum<cbag::path_style> {};

template <> struct convert<cbag::text_align> : public convert_enum<cbag::text_align> {};

template <> struct convert<cbag::font_t> : public convert_enum<cbag::font_t> {};

template <> struct convert<cbag::term_attr_type> : public convert_enum<cbag::term_attr_type> {};

template <> struct convert<cbag::text_disp_format> : public convert_enum<cbag::text_disp_format> {};

template <> struct convert<cbag::sig_type> : public convert_enum<cbag::sig_type> {};

template <> struct convert<cbag::term_type> : public convert_enum<cbag::term_type> {};

} // namespace YAML

#endif // CBAG_YAML_PRIMITIVES_H
