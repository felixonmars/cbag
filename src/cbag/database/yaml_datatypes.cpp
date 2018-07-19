/** \file yaml_datatypes.cpp
 *  \brief This file defines YAML serialization methods for datatypes.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

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
            return false;
        }
        try {
            switch (node[0].as<int>()) {
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
                    return false;
            }
        } catch (...) {
            return false;
        }
    }
}
