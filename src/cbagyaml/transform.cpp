/** \file primitives.cpp
 *  \brief This file defines YAML serialization methods for primitive objects
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <spdlog/spdlog.h>

#include <cbag/common/typedefs.h>

#include <cbagyaml/primitives.h>
#include <cbagyaml/fusion.h>

namespace YAML {

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

} // namespace YAML
