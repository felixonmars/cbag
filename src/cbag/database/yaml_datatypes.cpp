/** \file yaml_datatypes.cpp
 *  \brief This file defines YAML serialization methods for datatypes.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <easylogging++.h>

#include <cbag/database/yaml_datatypes.h>

namespace YAML {
Node convert<cbag::value_t>::encode(const cbag::value_t &rhs) {
    Node root;
    root.push_back(rhs.which());
    boost::apply_visitor(cbag::to_yaml_visitor(&root), rhs);
    return root;
}

bool convert<cbag::value_t>::decode(const Node &node, cbag::value_t &rhs) {
    if (!node.IsSequence() || node.size() != 2) {
        LOG(WARNING) << "cbag::value_t YAML decode: not a sequence or size != "
                        "2.  Node:\n"
                     << node;
        return false;
    }
    try {
        int value = node[0].as<int>();
        switch (value) {
        case 0:
            rhs = node[1].as<int32_t>();
            return true;
        case 1:
            rhs = node[1].as<double>();
            return true;
        case 2:
            rhs = node[1].as<bool>();
            return true;
        case 3:
            rhs = node[1].as<std::string>();
            return true;
        case 4:
            rhs = node[1].as<cbag::Time>();
            return true;
        case 5:
            rhs = node[1].as<cbag::Binary>();
            return true;
        default:
            LOG(WARNING)
                << "cbag::value_t YAML decode: unexpected which value: "
                << value << ".  Node:\n"
                << node;
            return false;
        }
    } catch (...) {
        LOG(WARNING) << "cbag::value_t YAML decode exception.  Node:\n" << node;
        return false;
    }
}
} // namespace YAML
