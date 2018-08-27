/** \file primitives.cpp
 *  \brief This file defines YAML serialization methods for primitive objects
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <spdlog/spdlog.h>

#include <cbag/yaml/fusion.h>
#include <cbag/yaml/primitives.h>

namespace YAML {
Node convert<cbag::point>::encode(const cbag::point &rhs) {
    Node root;
    root.push_back(rhs.x());
    root.push_back(rhs.y());
    return root;
}

bool convert<cbag::point>::decode(const Node &node, cbag::point &rhs) {
    auto logger = spdlog::get("cbag");
    if (!node.IsSequence() || node.size() != 2) {
        logger->warn("cbag::point YAML decode: not a sequence or size != 2.  Node:\n{}",
                     yaml::serialization::node_to_str(node));
        return false;
    }
    try {
        rhs.set(node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>());
    } catch (...) {
        logger->warn("cbag::point YAML decode exception.  Node:\n{}",
                     yaml::serialization::node_to_str(node));
        return false;
    }
    return true;
}

Node convert<cbag::transform>::encode(const cbag::transform &rhs) {
    Node root;
    root.push_back(rhs.xOffset());
    root.push_back(rhs.yOffset());
    root.push_back(static_cast<int>(rhs.orient()));
    return root;
}

bool convert<cbag::transform>::decode(const Node &node, cbag::transform &rhs) {
    auto logger = spdlog::get("cbag");
    if (!node.IsSequence() || node.size() != 3) {
        logger->warn("cbag::transform YAML decode: not a sequence or size != 3.  Node:\n{}",
                     yaml::serialization::node_to_str(node));
        return false;
    }
    try {
        rhs.set(node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>(),
                node[2].as<cbag::orientation>());
    } catch (...) {
        logger->warn("cbag::transform YAML decode exception.  Node:\n{}",
                     yaml::serialization::node_to_str(node));
        return false;
    }
    return true;
}

Node convert<cbag::box_t>::encode(const cbag::box_t &rhs) {
    Node root;
    root.push_back(rhs.left());
    root.push_back(rhs.bottom());
    root.push_back(rhs.right());
    root.push_back(rhs.top());
    return root;
}

bool convert<cbag::box_t>::decode(const Node &node, cbag::box_t &rhs) {
    auto logger = spdlog::get("cbag");
    if (!node.IsSequence() || node.size() != 4) {
        logger->warn("cbag::box_t YAML decode: not a sequence or size != 4.  Node:\n{}",
                     yaml::serialization::node_to_str(node));
        return false;
    }
    try {
        rhs.set(node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>(),
                node[2].as<cbag::coord_t>(), node[3].as<cbag::coord_t>());
    } catch (...) {
        logger->warn("cbag::box_t YAML decode exception.  Node:\n{}",
                     yaml::serialization::node_to_str(node));
        return false;
    }
    return true;
}

Node convert<cbag::point_array>::encode(const cbag::point_array &rhs) {
    Node root;
    uint32_t size = rhs.getNumElements();
    root["size"] = size;
    for (uint32_t i = 0; i < size; ++i) {
        root["points"].push_back(rhs[i]);
    }
    return root;
}

bool convert<cbag::point_array>::decode(const Node &node, cbag::point_array &rhs) {
    auto logger = spdlog::get("cbag");
    if (!node.IsMap()) {
        logger->warn("cbag::point_array YAML decode: not a map.  Node:\n{}",
                     yaml::serialization::node_to_str(node));
        return false;
    }
    try {
        auto size = node["size"].as<uint32_t>();
        Node pnode = node["points"];
        if (!pnode.IsSequence() || pnode.size() != size) {
            logger->warn("cbag::point_array YAML decode: not a sequence or size != {}.  Node:\n{}",
                         size, yaml::serialization::node_to_str(node));
            return false;
        }
        rhs.setSize(size);
        for (uint32_t i = 0; i < size; i++) {
            rhs[i] = pnode[i].as<cbag::point>();
        }
        rhs.setNumElements(size);
    } catch (...) {
        logger->warn("cbag::point_array YAML decode exception.  Node:\n{}",
                     yaml::serialization::node_to_str(node));
        return false;
    }
    return true;
}
} // namespace YAML
