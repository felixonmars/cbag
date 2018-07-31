/** \file yaml_cellviews.h
 *  \brief This file declares YAML serialization methods for cellviews.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */
#ifndef CBAG_DATABASE_YAML_CELLVIEWS_H
#define CBAG_DATABASE_YAML_CELLVIEWS_H

#include <cbag/yaml_fusion.h>

#include <cbag/database/cellviews_adapted.h>
#include <cbag/database/yaml_datatypes.h>
#include <cbag/database/yaml_figures.h>

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

#endif // CBAG_DATABASE_YAML_CELLVIEWS_H
