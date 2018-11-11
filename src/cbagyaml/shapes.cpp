/** \file shapes.cpp
 *  \brief This file defines YAML serialization methods for shapes.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <cbag/logging/logging.h>

#include <cbagyaml/box_t.h>
#include <cbagyaml/common.h>
#include <cbagyaml/enum.h>
#include <cbagyaml/shapes.h>
#include <cbagyaml/transformation.h>

namespace YAML {
Node convert<cbag::sch::shape_t>::encode(const cbag::sch::shape_t &rhs) {
    Node root;
    root.push_back(rhs.index());
    std::visit(cbagyaml::to_yaml_visitor(&root), rhs);
    return root;
}

bool convert<cbag::sch::shape_t>::decode(const Node &node, cbag::sch::shape_t &rhs) {
    auto logger = cbag::get_cbag_logger();
    if (!node.IsSequence() || node.size() != 2) {
        logger->warn("cbag::sch::shape_t YAML decode: not a sequence or size != 2.  Node:\n{}",
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
            rhs = node[1].as<cbag::sch::polygon>();
            return true;
        case 2:
            rhs = node[1].as<cbag::sch::arc>();
            return true;
        case 3:
            rhs = node[1].as<cbag::sch::donut>();
            return true;
        case 4:
            rhs = node[1].as<cbag::sch::ellipse>();
            return true;
        case 5:
            rhs = node[1].as<cbag::sch::line>();
            return true;
        case 6:
            rhs = node[1].as<cbag::sch::path>();
            return true;
        case 7:
            rhs = node[1].as<cbag::sch::text_t>();
            return true;
        case 8:
            rhs = node[1].as<cbag::sch::eval_text>();
            return true;
        default:
            logger->warn("cbag::sch::shape_t YAML decode: unexpected which value: {}.  Node:\n{}",
                         value, cbagyaml::node_to_str(node));
            return false;
        }
    } catch (...) {
        logger->warn("cbag::sch::shape_t YAML decode exception.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
}
} // namespace YAML
