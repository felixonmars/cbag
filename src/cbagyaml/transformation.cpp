/** \file primitives.cpp
 *  \brief This file defines YAML serialization methods for primitive objects
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <spdlog/spdlog.h>

#include <cbag/common/typedefs.h>

#include <cbagyaml/common.h>
#include <cbagyaml/enum.h>
#include <cbagyaml/transformation.h>

namespace YAML {

Node convert<cbag::transformation>::encode(const cbag::transformation &rhs) {
    Node root;
    root.push_back(rhs.xOffset());
    root.push_back(rhs.yOffset());
    root.push_back(rhs.orient_code());
    return root;
}

bool convert<cbag::transformation>::decode(const Node &node, cbag::transformation &rhs) {
    auto logger = spdlog::get("cbag");
    if (!node.IsSequence() || node.size() != 3) {
        logger->warn("cbag::transformation YAML decode: not a sequence or size != 3.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    try {
        rhs.set(node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>(), node[2].as<uint32_t>());
    } catch (...) {
        logger->warn("cbag::transformation YAML decode exception.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    return true;
}

} // namespace YAML
