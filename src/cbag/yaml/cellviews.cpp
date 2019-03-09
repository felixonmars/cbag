
#include <cbag/logging/logging.h>

#include <cbag/yaml/cellviews.h>

namespace YAML {

Node convert<cbag::sch::cellview_info>::encode(const cbag::sch::cellview_info &rhs) {
    Node root(NodeType::Map);
    root.force_insert("lib_name", rhs.lib_name);
    root.force_insert("cell_name", rhs.cell_name);
    root.force_insert("in_terms", rhs.in_terms);
    root.force_insert("out_terms", rhs.out_terms);
    root.force_insert("io_terms", rhs.io_terms);
    root.force_insert("nets", rhs.nets);
    root.force_insert("props", rhs.props);
    root.force_insert("is_prim", rhs.is_prim);
    if (!rhs.term_net_attrs.empty())
        root.force_insert("term_net_attrs", rhs.term_net_attrs);

    return root;
}

bool convert<cbag::sch::cellview_info>::decode(const Node &node, cbag::sch::cellview_info &rhs) {
    auto logger = cbag::get_cbag_logger();
    if (!node.IsMap()) {
        logger->warn("cbag::sch::cellview_info YAML decode: not a map.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
    try {
        rhs.lib_name = node["lib_name"].as<std::string>();
        rhs.cell_name = node["cell_name"].as<std::string>();
        rhs.in_terms = node["in_terms"].as<std::vector<std::string>>();
        rhs.out_terms = node["out_terms"].as<std::vector<std::string>>();
        rhs.io_terms = node["io_terms"].as<std::vector<std::string>>();
        rhs.nets = node["nets"].as<std::vector<std::string>>();
        rhs.props = node["props"].as<cbag::param_map>();
        rhs.is_prim = node["is_prim"].as<bool>();
        auto term_net_node = node["term_net_attrs"];
        if (term_net_node)
            rhs.term_net_attrs = term_net_node.as<cbag::util::sorted_map<
                std::string, cbag::util::sorted_map<std::string, std::string>>>();
        else
            rhs.term_net_attrs.clear();

        return true;
    } catch (...) {
        logger->warn("cbag::sch::cellview_info YAML decode exception.  Node:\n{}",
                     cbagyaml::node_to_str(node));
        return false;
    }
}

} // namespace YAML
