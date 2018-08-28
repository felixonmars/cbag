/** \file primitives.cpp
 *  \brief This file defines YAML serialization methods for primitive objects
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <spdlog/spdlog.h>

#include <cbag/common/typedefs.h>

#include <cbagyaml/box_t.h>
#include <cbagyaml/common.h>

namespace YAML {

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
                     cbagyaml::node_to_str(node));
        return false;
    }
    try {
        rhs.set(node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>(),
                node[2].as<cbag::coord_t>(), node[3].as<cbag::coord_t>());
    } catch (...) {
        logger->warn("cbag::box_t YAML decode exception.  Node:\n{}", cbagyaml::node_to_str(node));
        return false;
    }
    return true;
}

} // namespace YAML
