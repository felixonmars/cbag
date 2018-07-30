/** \file shapes.cpp
 *  \brief This file defines YAML serialization methods for shapes.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <easylogging++.h>

#include <cbag/database/yaml_datatypes.h>
#include <cbag/database/yaml_shapes.h>

namespace YAML {
Node convert<cbag::Shape>::encode(const cbag::Shape &rhs) {
    Node root;
    root.push_back(rhs.which());
    boost::apply_visitor(cbag::to_yaml_visitor(&root), rhs);
    return root;
}

bool convert<cbag::Shape>::decode(const Node &node, cbag::Shape &rhs) {
    if (!node.IsSequence() || node.size() != 2) {
        LOG(WARNING)
            << "cbag::shape YAML decode: not a sequence or size != 2.  Node:\n"
            << node;
        return false;
    }
    try {
        int value = node[0].as<int>();
        switch (value) {
        case 0:
            rhs = node[1].as<cbag::Rect>();
            return true;
        case 1:
            rhs = node[1].as<cbag::Poly>();
            return true;
        case 2:
            rhs = node[1].as<cbag::Arc>();
            return true;
        case 3:
            rhs = node[1].as<cbag::Donut>();
            return true;
        case 4:
            rhs = node[1].as<cbag::Ellipse>();
            return true;
        case 5:
            rhs = node[1].as<cbag::Line>();
            return true;
        case 6:
            rhs = node[1].as<cbag::Path>();
            return true;
        case 7:
            rhs = node[1].as<cbag::Text>();
            return true;
        case 8:
            rhs = node[1].as<cbag::EvalText>();
            return true;
        default:
            LOG(WARNING) << "cbag::Shape YAML decode: unexpected which value: "
                         << value << ".  Node:\n"
                         << node;
            return false;
        }
    } catch (...) {
        LOG(WARNING) << "cbag::Shape YAML decode exception.  Node:\n" << node;
        return false;
    }
}
} // namespace YAML
