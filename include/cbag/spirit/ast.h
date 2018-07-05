//
// Created by erichang on 7/4/18.
//

#ifndef CBAG_SPIRIT_AST_H
#define CBAG_SPIRIT_AST_H

#include <boost/optional/optional.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

#include <yaml-cpp/yaml.h>

namespace cbag {
    namespace spirit {
        namespace ast {

            namespace x3 = boost::spirit::x3;

            struct range : x3::position_tagged {

                uint32_t size() const;

                uint32_t get_stop() const;

                uint32_t get_stop_exclude() const;

                uint32_t start = 0;
                boost::optional<uint32_t> stop;
                uint32_t step = 1;
            };

            struct name_unit : x3::position_tagged {
                std::string base;
                boost::optional<range> index;
            };

            YAML::Emitter &operator<<(YAML::Emitter &out, const range &v);

            inline std::ostream &operator<<(std::ostream &os, range const &p) {
                os << '<' << p.start;
                if (p.stop)
                    os << ':' << *p.stop << ':';
                else
                    os << ":-:";
                return os << p.step << '>';
            }

            inline std::ostream &operator<<(std::ostream &os, name_unit const &p) {
                os << p.base;
                if (p.index)
                    os << *p.index;
                return os;
            }

        }
    }
}


#endif //CBAG_SPIRIT_AST_H
