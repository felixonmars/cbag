/** \file figures.cpp
 *  \brief This file defines YAML serialization methods for datatypes and
 * figures.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <spdlog/spdlog.h>

#include <cbag/database/yaml_datatypes.h>
#include <cbag/database/yaml_figures.h>

namespace YAML {
Node convert<std::variant<cbag::Rect, cbag::SchPinObject>>::encode(
    const std::variant<cbag::Rect, cbag::SchPinObject> &rhs) {
    Node root;
    root.push_back(rhs.index());
    std::visit(cbag::to_yaml_visitor(&root), rhs);
    return root;
}

bool convert<std::variant<cbag::Rect, cbag::SchPinObject>>::decode(
    const Node &node, std::variant<cbag::Rect, cbag::SchPinObject> &rhs) {
    auto logger = spdlog::get("cbag");
    if (!node.IsSequence() || node.size() != 2) {
        logger->warn("cbag::PinFigureObj YAML decode: not a sequence or size != 2.  Node:\n{}",
                     yaml::serialization::node_to_str(node));
        return false;
    }
    try {
        int value = node[0].as<int>();
        switch (value) {
        case 0:
            rhs = node[1].as<cbag::Rect>();
            return true;
        case 1:
            rhs = node[1].as<cbag::SchPinObject>();
            return true;
        default:
            logger->warn("cbag::PinFigureObj YAML decode: unexpected which value: {}.  Node:\n{}",
                         value, yaml::serialization::node_to_str(node));
            return false;
        }
    } catch (...) {
        logger->warn("cbag::PinFigureObj YAML decode exception.  Node:\n{}",
                     yaml::serialization::node_to_str(node));
        return false;
    }
}

} // namespace YAML
