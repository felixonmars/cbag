/** \file yaml_primitives.h
 *  \brief This file declares YAML serialization methods for primitive objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_DATABASE_YAML_PRIMITIVES_H
#define CBAG_DATABASE_YAML_PRIMITIVES_H

#include <yaml-cpp/yaml.h>

#include <cbag/database/primitives.h>

namespace YAML {
template <> struct convert<cbag::Orientation> {
    static Node encode(const cbag::Orientation &rhs) {
        Node n;
        n = static_cast<int>(rhs);
        return n;
    }

    static bool decode(const Node &node, cbag::Orientation &rhs) {
        if (node.IsScalar()) {
            try {
                rhs = static_cast<cbag::Orientation>(node.as<int>());
                return true;
            } catch (...) {
                return false;
            }
        } else {
            return false;
        }
    }
};

template <> struct convert<cbag::PathStyle> {
    static Node encode(const cbag::PathStyle &rhs) {
        Node n;
        n = static_cast<int>(rhs);
        return n;
    }

    static bool decode(const Node &node, cbag::PathStyle &rhs) {
        if (node.IsScalar()) {
            try {
                rhs = static_cast<cbag::PathStyle>(node.as<int>());
                return true;
            } catch (...) {
                return false;
            }
        } else {
            return false;
        }
    }
};

template <> struct convert<cbag::TextAlign> {
    static Node encode(const cbag::TextAlign &rhs) {
        Node n;
        n = static_cast<int>(rhs);
        return n;
    }

    static bool decode(const Node &node, cbag::TextAlign &rhs) {
        if (node.IsScalar()) {
            try {
                rhs = static_cast<cbag::TextAlign>(node.as<int>());
                return true;
            } catch (...) {
                return false;
            }
        } else {
            return false;
        }
    }
};

template <> struct convert<cbag::Font> {
    static Node encode(const cbag::Font &rhs) {
        Node n;
        n = static_cast<int>(rhs);
        return n;
    }

    static bool decode(const Node &node, cbag::Font &rhs) {
        if (node.IsScalar()) {
            try {
                rhs = static_cast<cbag::Font>(node.as<int>());
                return true;
            } catch (...) {
                return false;
            }
        } else {
            return false;
        }
    }
};

template <> struct convert<cbag::TermAttrType> {
    static Node encode(const cbag::TermAttrType &rhs) {
        Node n;
        n = static_cast<int>(rhs);
        return n;
    }

    static bool decode(const Node &node, cbag::TermAttrType &rhs) {
        if (node.IsScalar()) {
            try {
                rhs = static_cast<cbag::TermAttrType>(node.as<int>());
                return true;
            } catch (...) {
                return false;
            }
        } else {
            return false;
        }
    }
};

template <> struct convert<cbag::TextDispFormat> {
    static Node encode(const cbag::TextDispFormat &rhs) {
        Node n;
        n = static_cast<int>(rhs);
        return n;
    }

    static bool decode(const Node &node, cbag::TextDispFormat &rhs) {
        if (node.IsScalar()) {
            try {
                rhs = static_cast<cbag::TextDispFormat>(node.as<int>());
                return true;
            } catch (...) {
                return false;
            }
        } else {
            return false;
        }
    }
};

template <> struct convert<cbag::SigType> {
    static Node encode(const cbag::SigType &rhs) {
        Node n;
        n = static_cast<int>(rhs);
        return n;
    }

    static bool decode(const Node &node, cbag::SigType &rhs) {
        if (node.IsScalar()) {
            try {
                rhs = static_cast<cbag::SigType>(node.as<int>());
                return true;
            } catch (...) {
                return false;
            }
        } else {
            return false;
        }
    }
};

template <> struct convert<cbag::TermType> {
    static Node encode(const cbag::TermType &rhs) {
        Node n;
        n = static_cast<int>(rhs);
        return n;
    }

    static bool decode(const Node &node, cbag::TermType &rhs) {
        if (node.IsScalar()) {
            try {
                rhs = static_cast<cbag::TermType>(node.as<int>());
                return true;
            } catch (...) {
                return false;
            }
        } else {
            return false;
        }
    }
};

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

#endif // CBAG_DATABASE_YAML_PRIMITIVES_H
