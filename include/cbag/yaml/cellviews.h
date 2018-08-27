/** \file yaml_cellviews.h
 *  \brief This file declares YAML serialization methods for cellviews.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */
#ifndef CBAG_YAML_CELLVIEWS_H
#define CBAG_YAML_CELLVIEWS_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <yaml-cpp/yaml.h>

#include <cbag/schematic/cellviews.h>
#include <cbag/yaml/datatypes.h>
#include <cbag/yaml/figures.h>
#include <cbag/yaml/fusion.h>

BOOST_FUSION_ADAPT_STRUCT(cbag::SchCellView, lib_name, cell_name, view_name, bbox, in_terms,
                          out_terms, io_terms, shapes, instances, props, app_defs)

BOOST_FUSION_ADAPT_STRUCT(cbag::SchCellViewInfo, cell_name, in_terms, out_terms, io_terms, props)

// YAML conversion methods for std::unordered_map
namespace YAML {
template <typename K, typename V> struct convert<std::unordered_map<K, V>> {
    static Node encode(const std::unordered_map<K, V> &rhs) {
        Node node(NodeType::Map);
        for (auto const &x : rhs) {
            node.force_insert(x.first, x.second);
        }
        return node;
    }

    static bool decode(const Node &node, std::unordered_map<K, V> &rhs) {
        if (!node.IsMap())
            return false;

        rhs.clear();
        for (const_iterator it = node.begin(); it != node.end(); ++it)
#if defined(__GNUC__) && __GNUC__ < 4
            // workaround for GCC 3:
            rhs[it->first.template as<K>()] = it->second.template as<V>();
#else
            rhs[it->first.as<K>()] = it->second.as<V>();
#endif
        return true;
    }
};

} // namespace YAML

#endif // CBAG_YAML_CELLVIEWS_H
