//
// Created by erichang on 7/4/18.
//

#ifndef CBAG_SPIRIT_AST_H
#define CBAG_SPIRIT_AST_H

#include <vector>

#include <boost/optional/optional.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace cbag {
    namespace spirit {
        namespace ast {

            namespace x3 = boost::spirit::x3;

            /** Represents a range of indices at regular interval.
             *
             *  step size of 0 means that this range is empty; it doesn't contain any item.
             */
            struct range : x3::position_tagged {

                range()
                        : start(0), stop(0), step(0) {}

                range(uint32_t start, uint32_t stop, uint32_t step)
                        : start(start), stop(stop), step(step) {}

                uint32_t size() const;

                uint32_t get_stop_exclude() const;

                bool operator==(const range &other) const;

                bool operator<(const range &other) const;

                uint32_t start;
                uint32_t stop;
                uint32_t step;
            };

            /** Represents a single name
             *
             *  Could be either a scalar name ("foo") or a vector bit name ("bar<3>").
             */
            struct name_bit : x3::position_tagged {

                name_bit()
                        : base("") {}

                bool operator==(const name_bit &other) const;

                bool operator<(const name_bit &other) const;

                std::string base;
                boost::optional<uint32_t> index;
            };

            /** Represents a name unit
             *
             *  Possible formats are "foo", "bar<3:0>", "<*3>baz", or "<*3>asdf<3:0>".
             */
            struct name_unit : x3::position_tagged {

                name_unit()
                        : mult(1), base(""), index({0, 0, 0}) {}

                bool operator==(const name_unit &other) const;

                bool operator<(const name_unit &other) const;

                uint32_t mult;
                std::string base;
                range index;
            };

            /** Represents a list of name units.
             */
            struct name : x3::position_tagged {
                bool operator<(const name &other) const;

                std::vector<name_unit> unit_list;
            };
        }
    }
}


#endif //CBAG_SPIRIT_AST_H
