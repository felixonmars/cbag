/** \file figures.cpp
 *  \brief This file defines YAML serialization methods for datatypes and figures.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <easylogging++.h>

#include <cbag/database/yaml_datatypes.h>
#include <cbag/database/yaml_figures.h>


namespace YAML {
    Node convert<boost::variant<cbag::Rect, cbag::SchPinObject>>::encode(
            const boost::variant<cbag::Rect, cbag::SchPinObject> &rhs) {
        Node root;
        root.push_back(rhs.which());
        boost::apply_visitor(cbag::to_yaml_visitor(&root), rhs);
        return root;
    }

    bool convert<boost::variant<cbag::Rect, cbag::SchPinObject>>::decode(
            const Node &node, boost::variant<cbag::Rect, cbag::SchPinObject> &rhs) {
        if (!node.IsSequence() || node.size() != 2) {
            LOG(WARNING) << "cbag::PinFigureObj YAML decode: not a sequence or size != 2.  Node:\n"
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
                    rhs = node[1].as<cbag::SchPinObject>();
                    return true;
                default:
                    LOG(WARNING) << "cbag::PinFigureObj YAML decode: unexpected which value: "
                                 << value << ".  Node:\n" << node;
                    return false;
            }
        } catch (...) {
            LOG(WARNING) << "cbag::PinFigureObj YAML decode exception.  Node:\n" << node;
            return false;
        }
    }

}
