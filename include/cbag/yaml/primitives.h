/** \file yaml_primitives.h
 *  \brief This file declares YAML serialization methods for primitive objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_PRIMITIVES_H
#define CBAG_YAML_PRIMITIVES_H

#include <yaml-cpp/yaml.h>

#include <cbag/common/primitives.h>

namespace YAML {

template <typename T, typename std::enable_if_t<std::is_enum_v<T>> * = nullptr>
struct convert_enum {
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

template <> struct convert<cbag::Orientation> : public convert_enum<cbag::Orientation> {};

template <> struct convert<cbag::PathStyle> : public convert_enum<cbag::PathStyle> {};

template <> struct convert<cbag::TextAlign> : public convert_enum<cbag::TextAlign> {};

template <> struct convert<cbag::Font> : public convert_enum<cbag::Font> {};

template <> struct convert<cbag::TermAttrType> : public convert_enum<cbag::TermAttrType> {};

template <> struct convert<cbag::TextDispFormat> : public convert_enum<cbag::TextDispFormat> {};

template <> struct convert<cbag::SigType> : public convert_enum<cbag::SigType> {};

template <> struct convert<cbag::TermType> : public convert_enum<cbag::TermType> {};

template <> struct convert<cbag::Point> {
    static Node encode(const cbag::Point &rhs);

    static bool decode(const Node &node, cbag::Point &rhs);
};

template <> struct convert<cbag::Transform> {
    static Node encode(const cbag::Transform &rhs);

    static bool decode(const Node &node, cbag::Transform &rhs);
};

template <> struct convert<cbag::BBox> {
    static Node encode(const cbag::BBox &rhs);

    static bool decode(const Node &node, cbag::BBox &rhs);
};

template <> struct convert<cbag::PointArray> {
    static Node encode(const cbag::PointArray &rhs);

    static bool decode(const Node &node, cbag::PointArray &rhs);
};
} // namespace YAML

#endif // CBAG_YAML_PRIMITIVES_H
