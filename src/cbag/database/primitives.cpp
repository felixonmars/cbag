/** \file primitives.cpp
 *  \brief This file defines YAML serialization methods for primitive classes
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <yaml-cpp/yaml.h>

#include <cbag/database/primitives.h>

namespace YAML {
    template<>
    struct convert<cbag::Point> {
        // This function will only be available if the template parameter is a boost fusion sequence
        static Node encode(const cbag::Point &rhs) {
            Node root;
            root.push_back(rhs.x());
            root.push_back(rhs.y());
            return root;
        }

        // This function will only be available if the template parameter is a boost fusion sequence
        static bool decode(const Node &node, cbag::Point &rhs) {
            if (!node.IsSequence() || node.size() != 2) {
                return false;
            }
            try {
                rhs.set(node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>());
            } catch (...) {
                return false;
            }
            return true;
        }
    };

    template<>
    struct convert<cbag::Transform> {
        // This function will only be available if the template parameter is a boost fusion sequence
        static Node encode(const cbag::Transform &rhs) {
            Node root;
            root.push_back(rhs.xOffset());
            root.push_back(rhs.yOffset());
            root.push_back(static_cast<int>(rhs.orient()));
            return root;
        }

        // This function will only be available if the template parameter is a boost fusion sequence
        static bool decode(const Node &node, cbag::Transform &rhs) {
            if (!node.IsSequence() || node.size() != 3) {
                return false;
            }
            try {
                rhs.set(node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>(),
                        static_cast<cbag::Orientation >(node[2].as<int>()));
            } catch (...) {
                return false;
            }
            return true;
        }
    };

    template<>
    struct convert<cbag::BBox> {
        // This function will only be available if the template parameter is a boost fusion sequence
        static Node encode(const cbag::BBox &rhs) {
            Node root;
            root.push_back(rhs.left());
            root.push_back(rhs.bottom());
            root.push_back(rhs.right());
            root.push_back(rhs.top());
            return root;
        }

        // This function will only be available if the template parameter is a boost fusion sequence
        static bool decode(const Node &node, cbag::BBox &rhs) {
            if (!node.IsSequence() || node.size() != 4) {
                return false;
            }
            try {
                rhs.set(node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>(),
                        node[2].as<cbag::coord_t>(), node[3].as<cbag::coord_t>());
            } catch (...) {
                return false;
            }
            return true;
        }
    };

    template<>
    struct convert<cbag::PointArray> {
        // This function will only be available if the template parameter is a boost fusion sequence
        static Node encode(const cbag::PointArray &rhs) {
            Node root;
            uint32_t size = rhs.getNumElements();
            root["size"] = size;
            for (uint32_t i = 0; i < size; ++i) {
                root["points"].push_back(rhs[i]);
            }
            return root;
        }

        // This function will only be available if the template parameter is a boost fusion sequence
        static bool decode(const Node &node, cbag::PointArray &rhs) {
            if (!node.IsMap()) {
                return false;
            }
            try {
                auto size = node["size'"].as<uint32_t>();
                Node pnode = node["points"];
                if (!pnode.IsSequence() || pnode.size() != size) {
                    return false;
                }
                rhs.setSize(size);
                for (uint32_t i = 0; i < size; i++) {
                    rhs[i] = pnode[i].as<cbag::Point>();
                }
            } catch (...) {
                return false;
            }
            return true;
        }
    };
}

namespace yaml {
    namespace serialization {
        template<>
        std::string to_yaml<cbag::Point>(const cbag::Point &v) {
            YAML::Emitter emitter;
            YAML::Node root = YAML::convert<cbag::Point>::encode(v);
            emitter << root;
            return emitter.c_str();
        }

        template<>
        std::string to_yaml<cbag::Transform>(const cbag::Transform &v) {
            YAML::Emitter emitter;
            YAML::Node root = YAML::convert<cbag::Transform>::encode(v);
            emitter << root;
            return emitter.c_str();
        }

        template<>
        std::string to_yaml<cbag::BBox>(const cbag::BBox &v) {
            YAML::Emitter emitter;
            YAML::Node root = YAML::convert<cbag::BBox>::encode(v);
            emitter << root;
            return emitter.c_str();
        }

        template<>
        std::string to_yaml<cbag::PointArray>(const cbag::PointArray &v) {
            YAML::Emitter emitter;
            YAML::Node root = YAML::convert<cbag::PointArray>::encode(v);
            emitter << root;
            return emitter.c_str();
        }

        template<>
        bool from_yaml(std::string const &yaml_string, cbag::Point &obj) {
            YAML::Node n = YAML::Load(yaml_string);
            return YAML::convert<cbag::Point>::decode(n, obj);
        }

        template<>
        bool from_yaml(std::string const &yaml_string, cbag::Transform &obj) {
            YAML::Node n = YAML::Load(yaml_string);
            return YAML::convert<cbag::Transform>::decode(n, obj);
        }

        template<>
        bool from_yaml(std::string const &yaml_string, cbag::BBox &obj) {
            YAML::Node n = YAML::Load(yaml_string);
            return YAML::convert<cbag::BBox>::decode(n, obj);
        }

        template<>
        bool from_yaml(std::string const &yaml_string, cbag::PointArray &obj) {
            YAML::Node n = YAML::Load(yaml_string);
            return YAML::convert<cbag::PointArray>::decode(n, obj);
        }
    }
}