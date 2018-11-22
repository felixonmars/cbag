
#include <cbag/logging/logging.h>

#include <cbag/common/typedefs.h>

#include <cbag/yaml/box_t.h>
#include <cbag/yaml/common.h>

namespace YAML {

Node convert<cbag::box_t>::encode(const cbag::box_t &rhs) {
    Node root;
    root.push_back(rhs.xl());
    root.push_back(rhs.yl());
    root.push_back(rhs.xh());
    root.push_back(rhs.yh());
    return root;
}

bool convert<cbag::box_t>::decode(const Node &node, cbag::box_t &rhs) {
    auto logger = cbag::get_cbag_logger();
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
