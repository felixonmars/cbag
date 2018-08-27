/** \file yaml_datatypes.h
 *  \brief This file declares YAML serialization methods for datatypes.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_YAML_DATATYPES_H
#define CBAG_YAML_DATATYPES_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <yaml-cpp/yaml.h>

#include <cbag/common/datatypes.h>
#include <cbag/yaml/fusion.h>

BOOST_FUSION_ADAPT_STRUCT(cbag::Time, time_val)

BOOST_FUSION_ADAPT_STRUCT(cbag::Binary, name, bin_val)

namespace cbag {
class to_yaml_visitor {
  public:
    explicit to_yaml_visitor(YAML::Node *ptr) : ptr(ptr) {}

    template <typename T> void operator()(T &operand) const { ptr->push_back(operand); }

  private:
    YAML::Node *ptr;
};
} // namespace cbag

namespace YAML {
template <> struct convert<cbag::value_t> {
    static Node encode(const cbag::value_t &rhs);

    static bool decode(const Node &node, cbag::value_t &rhs);
};
} // namespace YAML

#endif // CBAG_YAML_DATATYPES_H
