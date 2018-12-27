
#include <cbag/logging/logging.h>

#include <cbag/common/transformation_util.h>

#include <cbag/yaml/common.h>
#include <cbag/yaml/enum.h>
#include <cbag/yaml/transformation.h>

namespace YAML {

Node convert<cbag::transformation>::encode(const cbag::transformation &rhs) {
    Node root(NodeType::Sequence);
    root.push_back(cbag::x(rhs));
    root.push_back(cbag::y(rhs));
    root.push_back(cbag::orient_code(rhs));
    return root;
}

bool convert<cbag::transformation>::decode(const Node &node, cbag::transformation &rhs) {
    auto logger = cbag::get_cbag_logger();
    if (!node.IsSequence() || node.size() != 3) {
        logger->warn("cbag::transformation YAML decode: not a sequence or size != 3.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    try {
        cbag::set_location(rhs, node[0].as<cbag::coord_t>(), node[1].as<cbag::coord_t>());
        cbag::set_orient_code(rhs, node[2].as<cbag::orient_t>());
    } catch (...) {
        logger->warn("cbag::transformation YAML decode exception.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    return true;
}

} // namespace YAML
