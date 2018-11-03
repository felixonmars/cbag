
#ifndef YAML_CPP_UNIQUE_PTR_H
#define YAML_CPP_UNIQUE_PTR_H

#include <memory>

#include <yaml-cpp/yaml.h>

// YAML conversion methods for std::unordered_map
namespace YAML {
template <typename V> struct convert<std::unique_ptr<V>> {
    static Node encode(const std::unique_ptr<V> &rhs) { return convert<V>::encode(*rhs); }

    static bool decode(const Node &node, std::unique_ptr<V> &rhs) {
        V *ptr = nullptr;
        try {
            ptr = new V(node.as<V>());
            rhs.reset(ptr);
        } catch (...) {
            if (ptr != nullptr)
                delete ptr;
            return false;
        }
        return true;
    }
};

} // namespace YAML

#endif
