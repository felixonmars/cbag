
#include <cbag/logging/logging.h>

#include <cbag/yaml/box_t.h>
#include <cbag/yaml/common.h>
#include <cbag/yaml/datatypes.h>
#include <cbag/yaml/enum.h>
#include <cbag/yaml/figures.h>
#include <cbag/yaml/shapes.h>
#include <cbag/yaml/transformation.h>

namespace YAML {

Node convert<cbag::sch::pin_fig_t>::encode(const cbag::sch::pin_fig_t &rhs) {
    Node root;
    root.push_back(rhs.index());
    std::visit(cbagyaml::to_yaml_visitor(&root), rhs);
    return root;
}

bool convert<cbag::sch::pin_fig_t>::decode(const Node &node, cbag::sch::pin_fig_t &rhs) {
    auto logger = cbag::get_cbag_logger();
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

Node convert<cbag::sch::pin_figure>::encode(const cbag::sch::pin_figure &rhs) {
    Node root(NodeType::Map);
    root.force_insert("obj", rhs.obj);
    root.force_insert("stype", rhs.stype);
    root.force_insert("ttype", rhs.ttype);
    if (!rhs.attrs.empty())
        root.force_insert("attrs", rhs.attrs);
    return root;
}

bool convert<cbag::sch::pin_figure>::decode(const Node &node, cbag::sch::pin_figure &rhs) {
    auto logger = cbag::get_cbag_logger();
    if (!node.IsMap()) {
        logger->warn("cbag::sch::pin_figure YAML decode: not a map.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    try {
        rhs.obj = node["obj"].as<cbag::sch::pin_fig_t>();
        rhs.stype = node["stype"].as<cbag::sig_type>();
        rhs.ttype = node["ttype"].as<cbag::term_type>();
        if (node["attrs"])
            rhs.attrs = node["attrs"].as<cbag::util::sorted_map<std::string, std::string>>();
        else
            rhs.attrs.clear();

        return true;
    } catch (...) {
        logger->warn("cbag::sch::pin_figure YAML decode exception.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
}

} // namespace YAML
