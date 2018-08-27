/** \file primitives.cpp
 *  \brief This file defines YAML serialization methods for primitive objects
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <spdlog/spdlog.h>

#include <cbag/common/typedefs.h>

#include <cbagyaml/fusion.h>
#include <cbagyaml/point.h>

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

} // namespace YAML
