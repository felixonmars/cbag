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
#include <iterator>
#include <string>
#include <variant>
#include <vector>

#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

namespace x3 = boost::spirit::x3;

namespace cbag {
namespace spirit {

struct namespace_info;

namespace ast {

/** Represents a range of indices at regular interval.
 *
 *  step size of 0 means that this range is empty; it doesn't contain any item.
 *  the step field is always non-negative.  However, if stop < start, then it is
 *  implied that the index decreases.
 *
 *  the stop field is inclusive.  However, get_stop_exclude() method will return
 *  an exclusive stop value if needed.
 */
struct range : x3::position_tagged {
  public:
    uint32_t start = 0;
    uint32_t stop = 0;
    uint32_t step = 0;

    class const_iterator {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = uint32_t;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = const value_type &;

      private:
        uint32_t val_ = 0;
        uint32_t step_ = 0;
        bool up_ = false;

      public:
        const_iterator();

        const_iterator(uint32_t val, uint32_t step, bool up);

        const_iterator &operator++();
        reference operator*() const;
        bool operator==(const const_iterator &rhs) const;
        bool operator!=(const const_iterator &rhs) const;
    };

    range();

    range(uint32_t start, uint32_t stop, uint32_t step);

    bool empty() const;

    uint32_t size() const;

    uint32_t get_stop_exclude() const;

    const_iterator begin() const;

    const_iterator end() const;

    // no bounds checking
    uint32_t operator[](uint32_t index) const;

    std::string to_string(const namespace_info &ns, bool show_stop = false) const;
};

/** Represents a unit name; either a scalar or vector name.
 *
 *  POssible formats are "foo", "bar[2]", "baz[3:1]"
 *
 *  An empty base means that this name_unit is empty.
 */
struct name_unit : x3::position_tagged {
  public:
    std::string base;
    range idx_range;

    name_unit();

    name_unit(std::string base, range idx_range);

    bool empty() const;

    uint32_t size() const;

    bool is_vector() const;

    std::string to_string(const namespace_info &ns) const;

    // precondition: 0 <= idx < size
    std::string get_name_bit(const namespace_info &ns, uint32_t index) const;
};

struct name_rep;

/** Represents a list of name_rep's.
 *
 *  An empty rep_list means that this name is empty.
 */
struct name : x3::position_tagged {
  public:
    std::vector<name_rep> rep_list;

    name();

    name(std::vector<name_rep> rep_list);

    bool empty() const;

    uint32_t size() const;

    std::string to_string(const namespace_info &ns) const;
};

/** Represents a repeated name
 *
 *  Possible formats are "<*3>foo", "<*3>(a,b)", or just name_unit.
 *  A mult of 0 means this name_rep is empty.
 */
struct name_rep : x3::position_tagged {
  public:
    uint32_t mult = 1;
    std::variant<name_unit, name> data;

    name_rep();

    name_rep(uint32_t mult, std::variant<name_unit, name> data);

    name_rep(uint32_t mult, name_unit nu);

    bool empty() const;

    uint32_t size() const;

    uint32_t data_size() const;

    bool is_vector() const;

    std::string to_string(const namespace_info &ns) const;

    std::vector<std::string> data_name_bits(const namespace_info &ns) const;
};

} // namespace ast
} // namespace spirit
} // namespace cbag

#endif // CBAG_SPIRIT_AST_H
