/** \file ast.h
 *  \brief This file defines various abstract syntax tree (AST) structures parsed by Spirit X3.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_AST_H
#define CBAG_SPIRIT_AST_H

#include <cstdint>
#include <string>
#include <vector>

#include <boost/optional/optional.hpp>
#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>


namespace x3 = boost::spirit::x3;

namespace cbag {
    namespace spirit {
        namespace ast {

            /** Represents a range of indices at regular interval.
             *
             *  step size of 0 means that this range is empty; it doesn't contain any item.
             */
            struct range : x3::position_tagged {
                range();

                range(uint32_t start, uint32_t stop, uint32_t step);

                uint32_t size() const;

                uint32_t get_stop_exclude() const;

                uint32_t operator[](uint32_t index) const;

                bool operator==(const range &other) const;

                bool operator!=(const range &other) const;

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
                using optint_t = boost::optional<uint32_t>;

                name_bit();

                explicit name_bit(std::string base);

                name_bit(std::string base, uint32_t index);

                bool operator==(const name_bit &other) const;

                bool operator!=(const name_bit &other) const;

                bool operator<(const name_bit &other) const;

                std::string base;
                optint_t index;
            };

            /** Represents a name unit
             *
             *  Possible formats are "foo", "bar<3:0>", "<*3>baz", or "<*3>asdf<3:0>".
             */
            struct name_unit : x3::position_tagged {

                name_unit();

                uint32_t size() const;

                bool is_vector() const;

                name_bit operator[](uint32_t index) const;

                bool operator==(const name_unit &other) const;

                bool operator!=(const name_unit &other) const;

                bool operator<(const name_unit &other) const;

                uint32_t mult;
                std::string base;
                range idx_range;
            };

            /** Represents a list of name units.
             */
            struct name : x3::position_tagged {
                using namelist_t = std::vector<name_unit>;

                class const_iterator {
                public:
                    const_iterator(const name *ptr, unsigned long unit_index, uint32_t bit_index);

                    const_iterator &operator++();

                    bool operator!=(const const_iterator &other) const;

                    bool operator==(const const_iterator &other) const;

                    name_bit operator*() const;

                private:
                    const name *ptr;
                    unsigned long unit_index;
                    uint32_t bit_index;
                };

                const_iterator begin() const;

                const_iterator end() const;

                bool operator==(const name &other) const;

                bool operator!=(const name &other) const;

                bool operator<(const name &other) const;

                namelist_t unit_list;
            };
        }
    }
}


#endif //CBAG_SPIRIT_AST_H
