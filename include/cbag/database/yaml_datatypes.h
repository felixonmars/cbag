/** \file yaml_datatypes.h
 *  \brief This file declares YAML serialization methods for datatypes.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_DATABASE_YAML_DATATYPES_H
#define CBAG_DATABASE_YAML_DATATYPES_H

#include <boost/variant.hpp>

#include <yaml-cpp/yaml.h>

#include <cbag/yaml_fusion.h>
#include <cbag/database/datatypes_adapted.h>


namespace cbag {
    class to_yaml_visitor : public boost::static_visitor<> {
    public:
        explicit to_yaml_visitor(YAML::Node *ptr) : ptr(ptr) {}

        template<typename T>
        void operator()(T &operand) const {
            ptr->push_back(operand);
        }

    private:

        YAML::Node *ptr;
    };
}

namespace YAML {
    template<>
    struct convert<cbag::value_t> {
        static Node encode(const cbag::value_t &rhs);

        static bool decode(const Node &node, cbag::value_t &rhs);
    };
}


#endif //CBAG_DATABASE_YAML_DATATYPES_H
