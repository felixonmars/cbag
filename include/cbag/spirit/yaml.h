//
// Created by erichang on 7/5/18.
//

#ifndef CBAG_SPIRIT_YAML_H
#define CBAG_SPIRIT_YAML_H

#include <yaml-cpp/yaml.h>

#include <cbag/spirit/ast.h>


namespace cbag {
    namespace spirit {
        namespace ast {

            inline YAML::Emitter &operator<<(YAML::Emitter &out, const range &v) {
                return out << YAML::Flow << YAML::BeginSeq << v.start << v.stop << v.step
                           << YAML::EndSeq;
            }

            inline YAML::Emitter &operator<<(YAML::Emitter &out, const name_bit &v) {
                out << YAML::Flow << YAML::BeginSeq << v.base;
                if (v.index) {
                    out << *v.index;
                } else {
                    out << YAML::Null;
                }
                return out << YAML::EndSeq;
            }

            inline YAML::Emitter &operator<<(YAML::Emitter &out, const name_unit &v) {
                return out << YAML::Flow << YAML::BeginSeq << v.base << v.idx_range << v.mult
                           << YAML::EndSeq;
            }

            inline YAML::Emitter &operator<<(YAML::Emitter &out, const name &v) {
                return out << v.unit_list;
            }
        }
    }
}

#endif //CBAG_SPIRIT_YAML_H
