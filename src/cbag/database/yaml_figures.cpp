/** \file figures.cpp
 *  \brief This file defines YAML serialization methods for datatypes and figures.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#include <cbag/database/yaml_figures.h>


namespace YAML {
    Node convert<cbag::Shape>::encode(const cbag::Shape &rhs) {
        Node root;
        root.push_back(rhs.which());
        boost::apply_visitor(cbag::to_yaml_visitor(&root), rhs);
        return root;
    }

    bool convert<cbag::Shape>::decode(const Node &node, cbag::Shape &rhs) {
        if (!node.IsSequence() || node.size() != 2) {
            return false;
        }
        try {
            switch (node[0].as<int>()) {
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
                    return false;
            }
        } catch (...) {
            return false;
        }
    }

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
            return false;
        }
        try {
            switch (node[0].as<int>()) {
                case 0:
                    rhs = node[1].as<cbag::Rect>();
                    return true;
                case 1:
                    rhs = node[1].as<cbag::SchPinObject>();
                    return true;
                default:
                    return false;
            }
        } catch (...) {
            return false;
        }
    }

}
