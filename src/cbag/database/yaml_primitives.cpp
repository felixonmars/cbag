/** \file primitives.cpp
 *  \brief This file defines YAML serialization methods for primitive objects
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <easylogging++.h>

#include <cbag/database/yaml_primitives.h>


namespace YAML {
    Node convert<cbag::Point>::encode(const cbag::Point &rhs) {
        Node root;
        root.push_back(rhs.x());
        root.push_back(rhs.y());
        return root;
    }

    bool convert<cbag::Point>::decode(const Node &node, cbag::Point &rhs) {
        if (!node.IsSequence() || node.size() != 2) {
            LOG(WARNING) << "cbag::Point YAML decode: not a sequence or size != 2.  Node:\n"
                         << node;
            return false;
        }
        try {
            rhs.set(node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>());
        } catch (...) {
            LOG(WARNING) << "cbag::Point YAML decode exception.  Node:\n" << node;
            return false;
        }
        return true;
    }

    Node convert<cbag::Transform>::encode(const cbag::Transform &rhs) {
        Node root;
        root.push_back(rhs.xOffset());
        root.push_back(rhs.yOffset());
        root.push_back(static_cast<int>(rhs.orient()));
        return root;
    }

    bool convert<cbag::Transform>::decode(const Node &node, cbag::Transform &rhs) {
        if (!node.IsSequence() || node.size() != 3) {
            LOG(WARNING) << "cbag::Transform YAML decode: not a sequence or size != 3.  Node:\n"
                         << node;
            return false;
        }
        try {
            rhs.set(node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>(),
                    static_cast<cbag::Orientation >(node[2].as<int>()));
        } catch (...) {
            LOG(WARNING) << "cbag::Transform YAML decode exception.  Node:\n" << node;
            return false;
        }
        return true;
    }

    Node convert<cbag::BBox>::encode(const cbag::BBox &rhs) {
        Node root;
        root.push_back(rhs.left());
        root.push_back(rhs.bottom());
        root.push_back(rhs.right());
        root.push_back(rhs.top());
        return root;
    }

    bool convert<cbag::BBox>::decode(const Node &node, cbag::BBox &rhs) {
        if (!node.IsSequence() || node.size() != 4) {
            LOG(WARNING) << "cbag::BBox YAML decode: not a sequence or size != 4.  Node:\n" << node;
            return false;
        }
        try {
            rhs.set(node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>(),
                    node[2].as<cbag::coord_t>(), node[3].as<cbag::coord_t>());
        } catch (...) {
            LOG(WARNING) << "cbag::BBox YAML decode exception.  Node:\n" << node;
            return false;
        }
        return true;
    }

    Node convert<cbag::PointArray>::encode(const cbag::PointArray &rhs) {
        Node root;
        uint32_t size = rhs.getNumElements();
        root["size"] = size;
        for (uint32_t i = 0; i < size; ++i) {
            root["points"].push_back(rhs[i]);
        }
        return root;
    }

    bool convert<cbag::PointArray>::decode(const Node &node, cbag::PointArray &rhs) {
        if (!node.IsMap()) {
            LOG(WARNING) << "cbag::PointArray YAML decode: not a map.  Node:\n"
                         << node;
            return false;
        }
        try {
            auto size = node["size"].as<uint32_t>();
            Node pnode = node["points"];
            if (!pnode.IsSequence() || pnode.size() != size) {
                LOG(WARNING) << "cbag::PointArray YAML decode: not a sequence or size !="
                             << size << ".  Node:\n" << node;
                return false;
            }
            rhs.setSize(size);
            for (uint32_t i = 0; i < size; i++) {

                rhs[i] = pnode[i].as<cbag::Point>();
            }
        } catch (...) {
            LOG(WARNING) << "cbag::PointArray YAML decode exception.  Node:\n" << node;
            return false;
        }
        return true;
    }
}
