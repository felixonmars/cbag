#ifndef CBAG_YAML_INT_ARRAY_H
#define CBAG_YAML_INT_ARRAY_H

#include <array>

#include <yaml-cpp/yaml.h>

#include <cbag/yaml/common.h>

namespace YAML {

template <typename T, unsigned int N> struct convert_arr {
    static Node encode(const std::array<T, N> &rhs) {
        Node ans(NodeType::Sequence);
        for (decltype(N) idx = 0; idx < N; ++idx) {
            ans.push_back(rhs[idx]);
        }
        return ans;
    }

    static bool decode(const Node &node, std::array<T, N> &rhs) {
        if (!node.IsSequence() || node.size() != N)
            return false;
        for (decltype(N) idx = 0; idx < N; ++idx) {
            rhs[idx] = cbagyaml::get_int<T>(node[idx]);
        }
        return true;
    }
};

} // namespace YAML

#endif
