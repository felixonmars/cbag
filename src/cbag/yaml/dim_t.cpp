
#include <cbag/logging/logging.h>

#include <cbag/common/typedefs.h>

#include <cbag/yaml/common.h>
#include <cbag/yaml/dim_t.h>

namespace YAML {

Node convert<cbag::dim_t>::encode(const cbag::dim_t &rhs) {
    Node root(NodeType::Sequence);
    root.push_back(rhs[0]);
    root.push_back(rhs[1]);
    return root;
}

bool convert<cbag::dim_t>::decode(const Node &node, cbag::dim_t &rhs) {
    auto logger = cbag::get_cbag_logger();
    if (!node.IsSequence() || node.size() != 2) {
        logger->warn("cbag::dim_t YAML decode: not a sequence or size != 2.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    try {
        rhs[0] = cbagyaml::get_int<cbag::dist_t>(node[0]);
        rhs[1] = cbagyaml::get_int<cbag::dist_t>(node[1]);
    } catch (...) {
        logger->warn("cbag::dim_t YAML decode exception.  Node:\n{}", cbagyaml::node_to_str(node));
        return false;
    }
    return true;
}

} // namespace YAML
