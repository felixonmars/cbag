
#ifndef YAML_CPP_TUPLE_H
#define YAML_CPP_TUPLE_H

#include <tuple>
#include <type_traits>

#include <yaml-cpp/yaml.h>

// code to iterate over tuple, from:
// https://stackoverflow.com/questions/26902633/how-to-iterate-over-a-stdtuple-in-c-11/26902803
template <class F, class... Ts, std::size_t... Is>
void for_each_in_tuple(const std::tuple<Ts...> &tuple, F func, std::index_sequence<Is...>) {
    using expander = int[];
    (void)expander{0, ((void)func(std::get<Is>(tuple)), 0)...};
}

template <class F, class... Ts, std::size_t... Is>
void set_each_in_tuple(std::tuple<Ts...> &tuple, F func, std::index_sequence<Is...>) {
    using expander = int[];
    (void)expander{0, (std::get<Is>(tuple) = func(Is).template as<Ts>(), 0)...};
}

// YAML conversion methods for std::unordered_map
namespace YAML {
template <typename... Args> struct convert<std::tuple<Args...>> {
    static Node encode(const std::tuple<Args...> &rhs) {
        Node root(NodeType::Sequence);
        for_each_in_tuple(rhs, [&root](const auto &x) { root.push_back(x); },
                          std::make_index_sequence<sizeof...(Args)>());
        return root;
    }

    static bool decode(const Node &node, std::tuple<Args...> &rhs) {
        if (!node.IsSequence() ||
            node.size() != std::tuple_size_v<std::remove_reference_t<decltype(rhs)>>)
            return false;

        set_each_in_tuple(rhs, [&node](std::size_t idx) { return node[idx]; },
                          std::make_index_sequence<sizeof...(Args)>());
        return true;
    }
};

} // namespace YAML

#endif
