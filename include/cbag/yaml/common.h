/** \file yaml_primitives.h
 *  \brief This file declares YAML serialization methods for primitive objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_COMMON_H
#define CBAG_YAML_COMMON_H

#include <string>
#include <unordered_map>

#include <yaml-cpp/yaml.h>

namespace cbagyaml {

class to_yaml_visitor {
  private:
    YAML::Node *ptr;

  public:
    explicit to_yaml_visitor(YAML::Node *ptr) : ptr(ptr) {}

    template <typename T> void operator()(T &operand) const { ptr->push_back(operand); }
};

inline std::string node_to_str(const YAML::Node &node) {
    YAML::Emitter emitter;
    emitter << node;
    return emitter.c_str();
}

template <typename Base> std::string to_yaml(const Base &v) {
    YAML::Emitter emitter;
    YAML::Node root = YAML::convert<Base>::encode(v);
    emitter << root;
    return emitter.c_str();
}

// Load yaml into this object
template <typename Base> bool from_yaml(std::string const &yaml_string, Base &obj) {
    YAML::Node n;
    try {
        n = YAML::Load(yaml_string);
    } catch (...) {
        return false;
    }
    return YAML::convert<Base>::decode(n, obj);
}

template <typename T> T get_int(const YAML::Node &val) {
    if (val.as<double>() == std::numeric_limits<double>::infinity()) {
        return std::numeric_limits<T>::max();
    } else {
        return val.as<T>();
    }
}

template <typename T> std::tuple<int, std::vector<T>> int_map_to_vec(const YAML::Node &node) {
    std::unordered_map<int, T> tmp;
    auto min_key = std::numeric_limits<int>::max();
    for (const auto &child : node) {
        auto cur_key = child.first.as<int>();
        min_key = std::min(min_key, cur_key);
        tmp.emplace(cur_key, child.second.as<T>());
    }
    auto num = static_cast<decltype(min_key)>(tmp.size());
    std::vector<T> vec;
    vec.reserve(num);
    for (auto key = min_key; key < min_key + num; ++key) {
        auto iter = tmp.find(key);
        if (iter == tmp.end())
            throw std::runtime_error("Cannot find integer key " + std::to_string(key));
        vec.push_back(std::move(iter->second));
    }
    return {min_key, vec};
}

} // namespace cbagyaml

#endif
