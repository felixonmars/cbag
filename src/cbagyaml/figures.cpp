/** \file figures.cpp
 *  \brief This file defines YAML serialization methods for datatypes and
 * figures.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <spdlog/spdlog.h>

#include <cbagyaml/box_t.h>
#include <cbagyaml/common.h>
#include <cbagyaml/datatypes.h>
#include <cbagyaml/enum.h>
#include <cbagyaml/figures.h>
#include <cbagyaml/point.h>
#include <cbagyaml/point_array.h>
#include <cbagyaml/shapes.h>
#include <cbagyaml/transformation.h>

namespace YAML {
Node convert<cbag::sch::pin_fig_t>::encode(const cbag::sch::pin_fig_t &rhs) {
    Node root;
    root.push_back(rhs.index());
    std::visit(cbagyaml::to_yaml_visitor(&root), rhs);
    return root;
}

bool convert<cbag::sch::pin_fig_t>::decode(const Node &node, cbag::sch::pin_fig_t &rhs) {
    auto logger = spdlog::get("cbag");
    if (!node.IsSequence() || node.size() != 2) {
        logger->warn("cbag::sch::pin_fig_t YAML decode: not a sequence or size != 2.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    try {
        int value = node[0].as<int>();
        switch (value) {
        case 0:
            rhs = node[1].as<cbag::sch::rectangle>();
            return true;
        case 1:
            rhs = node[1].as<cbag::sch::pin_object>();
            return true;
        default:
            logger->warn("cbag::sch::pin_fig_t YAML decode: unexpected which value: {}.  Node:\n{}",
                         value, cbagyaml::node_to_str(node));
            return false;
        }
    } catch (...) {
        logger->warn("cbag::sch::pin_fig_t YAML decode exception.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
}

} // namespace YAML
