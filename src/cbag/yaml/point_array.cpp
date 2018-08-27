/** \file primitives.cpp
 *  \brief This file defines YAML serialization methods for primitive objects
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <spdlog/spdlog.h>

#include <cbag/yaml/fusion.h>
#include <cbag/yaml/point.h>
#include <cbag/yaml/point_array.h>

namespace YAML {

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
