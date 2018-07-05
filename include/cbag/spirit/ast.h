//
// Created by erichang on 7/4/18.
//

#ifndef CBAG_SPIRIT_AST_H
#define CBAG_SPIRIT_AST_H

#include <boost/optional/optional.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace cbag {
    namespace spirit {
        namespace ast {

            namespace x3 = boost::spirit::x3;

            struct range : x3::position_tagged {
                unsigned int start = 0;
                boost::optional<unsigned int> stop;
                boost::optional<unsigned int> step;
            };

            struct name_unit : x3::position_tagged {
                std::string base;
                boost::optional<range> index;
            };

            inline std::ostream &operator<<(std::ostream &os, range const &p) {
                os << '<' << p.start;
                if (p.stop)
                    os << ':' << *p.stop;
                if (p.step)
                    os << ':' << *p.step;
                return os << '>';
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
