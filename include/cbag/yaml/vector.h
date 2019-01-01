/** \file yaml_primitives.h
 *  \brief This file declares YAML serialization methods for primitive objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_INT_ARRAY_H
#define CBAG_YAML_INT_ARRAY_H

#include <array>

#include <yaml-cpp/yaml.h>

#include <cbag/common/vector.h>
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

template <> struct convert<cbag::vector> : public convert_arr<cbag::offset_t, 2> {};

} // namespace YAML

#endif
