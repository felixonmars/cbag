/** \file ast.h
 *  \brief This file defines various abstract syntax tree (AST) structures
 * parsed by Spirit X3.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_AST_H
#define CBAG_SPIRIT_AST_H

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

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
    uint32_t start = 0;
    uint32_t stop = 0;
    uint32_t step = 0;

    range();

    range(uint32_t start, uint32_t stop, uint32_t step);

    uint32_t size() const;

    uint32_t get_stop_exclude() const;

    uint32_t operator[](uint32_t index) const;

    bool operator==(const range &other) const;

    bool operator!=(const range &other) const;

    bool operator<(const range &other) const;
};

/** Represents a single name
 *
 *  Could be either a scalar name ("foo") or a vector bit name ("bar<3>").
 */
struct name_bit : x3::position_tagged {
    std::string base;
    std::optional<uint32_t> index;

    name_bit();

    explicit name_bit(std::string base);

    name_bit(std::string base, uint32_t index);

    bool operator==(const name_bit &other) const;

    bool operator!=(const name_bit &other) const;

    bool operator<(const name_bit &other) const;
};

/** Represents a unit name; either a scalar or vector name.
 *
 *  POssible formats are "foo", "bar[2]", "baz[3:1]"
 */
struct name_unit : x3::position_tagged {
    std::string base;
    range idx_range;

    name_unit();

    uint32_t size() const;

    bool is_vector() const;

    name_bit operator[](uint32_t index) const;

    bool operator==(const name_unit &other) const;

    bool operator!=(const name_unit &other) const;

    bool operator<(const name_unit &other) const;
};

/** Represents a repeated name
 *
 *  Possible formats are "<*3>foo", "<*3>(a,b)", or just name_unit.
 */
struct name_rep : x3::position_tagged {
    uint32_t mult = 1;
    std::string base;
    range idx_range;

    name_rep();

    uint32_t size() const;

    bool is_vector() const;

    name_bit operator[](uint32_t index) const;

    bool operator==(const name_rep &other) const;

    bool operator!=(const name_rep &other) const;

    bool operator<(const name_rep &other) const;
};

/** Represents a list of name_rep's.
 */
struct name : x3::position_tagged {
    class const_iterator {
      private:
        const name *ptr = nullptr;
        unsigned long unit_index = 0;
        uint32_t bit_index = 0;

      public:
        const_iterator(const name *ptr, unsigned long unit_index, uint32_t bit_index);

        const_iterator &operator++();

        bool operator!=(const const_iterator &other) const;

        bool operator==(const const_iterator &other) const;

        name_bit operator*() const;
    };

    std::vector<name_rep> rep_list;

    name();

    const_iterator begin() const;

    const_iterator end() const;

    uint32_t size() const;

    bool operator==(const name &other) const;

    bool operator!=(const name &other) const;

    bool operator<(const name &other) const;
};

} // namespace ast
} // namespace spirit
} // namespace cbag

#endif // CBAG_SPIRIT_AST_H
