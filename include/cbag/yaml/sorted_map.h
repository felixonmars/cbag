
#ifndef CBAG_YAML_SORTED_MAP_H
#define CBAG_YAML_SORTED_MAP_H

#include <cbag/util/sorted_map.h>

#include <yaml-cpp/yaml.h>

// YAML conversion methods for std::unordered_map
namespace YAML {
template <typename K, typename V> struct convert<cbag::util::sorted_map<K, V>> {
    static Node encode(const cbag::util::sorted_map<K, V> &rhs) {
        Node node(NodeType::Map);
        for (auto const &x : rhs) {
            node.force_insert(x.first, x.second);
        }
        return node;
    }

    static bool decode(const Node &node, cbag::util::sorted_map<K, V> &rhs) {
        if (!node.IsMap())
            return false;

        rhs.clear();
        for (const_iterator it = node.begin(); it != node.end(); ++it)
#if defined(__GNUC__) && __GNUC__ < 4
            // workaround for GCC 3:
            rhs.insert_or_assign(it->first.template as<K>(), it->second.template as<V>());
#else
            rhs.insert_or_assign(it->first.as<K>(), it->second.as<V>());
#endif
        return true;
    }
};

} // namespace YAML

#endif
