/** \file yaml.h
 *  \brief This file defines functions to write various database objects to YAML file.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */


#ifndef CBAG_DATABASE_YAML_H
#define CBAG_DATABASE_YAML_H

#include <yaml-cpp/yaml.h>

#include <cbag/database/datatypes.h>
#include <cbag/database/geometries.h>
#include <cbag/database/shapes.h>
#include <cbag/database/cellviews.h>


namespace cbag {

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const Transform &v) {
        return out << YAML::Flow
                   << YAML::BeginSeq << v.x << v.y << v.orient._to_string() << YAML::EndSeq;
    }

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const Time &v) {
        return out << YAML::Flow << YAML::BeginSeq << "time" << v.time_val << YAML::EndSeq;
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const SchInstance &v);

    YAML::Emitter &operator<<(YAML::Emitter &out, const SchMaster &v);

}

namespace YAML {
    template<>
    struct convert<cbag::Transform> {
        static Node encode(const cbag::Transform& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.orient._to_string());
            return node;
        }

        static bool decode(const Node& node, cbag::Transform& rhs) {
            if(!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.x = node[0].as<cbag::coord_t>();
            rhs.y = node[1].as<cbag::coord_t>();
            rhs.orient = cbag::Orientation::_from_string(node[2].as<std::string>().c_str());
            return true;
        }
    };
}


// define YAML overload for cbag::value_t in boost namespace, because
// cbag::value_t is a typedef of a boost data structure.
namespace boost {
    /** A visitor struct for writing boost:variant to a YAML file.
     */
    struct VarYAMLVisitor : public static_visitor<> {

        explicit VarYAMLVisitor(YAML::Emitter *out_ptr)
                : out_ptr(out_ptr) {}

        void operator()(const int32_t &i) const {
            (*out_ptr) << i;
        }

        void operator()(const double &d) const {
            (*out_ptr) << d;
        }

        void operator()(const std::string &s) const {
            (*out_ptr) << YAML::DoubleQuoted << s;
        }

        void operator()(const cbag::Time &t) const {
            (*out_ptr) << t;
        }

        void operator()(const std::vector<unsigned char> &v) const {
            (*out_ptr) << YAML::Binary(v.data(), v.size());
        }

        YAML::Emitter *out_ptr;

    };

    inline YAML::Emitter &operator<<(YAML::Emitter &out, const cbag::value_t &v) {
        VarYAMLVisitor visitor(&out);
        boost::apply_visitor(visitor, v);
        return out;
    }
}

#endif //CBAG_DATABASE_YAML_H
