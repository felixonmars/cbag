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
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <fmt/core.h>

#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

#include <cbag/spirit/namespace_info.h>
#include <cbag/spirit/variant_support.h>

namespace x3 = boost::spirit::x3;

namespace cbag {
namespace spirit {
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
  private:
    mutable std::optional<uint32_t> size_;

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

    uint32_t size() const;

    uint32_t get_stop_exclude() const;

    const_iterator begin() const;

    const_iterator end() const;

    uint32_t operator[](uint32_t index) const;

    uint32_t at(uint32_t index) const;

    std::string to_string(const namespace_info &ns, bool show_stop = false) const;
};

/** Represents a unit name; either a scalar or vector name.
 *
 *  POssible formats are "foo", "bar[2]", "baz[3:1]"
 */
struct name_unit : x3::position_tagged {
  public:
    std::string base;
    range idx_range;

    name_unit();

    uint32_t size() const;

    bool is_vector() const;

    std::string to_string(const namespace_info &ns) const;

    std::string get_name_bit(const namespace_info &ns, uint32_t index) const;

    template <class OutIter> void append_name_bits(const namespace_info &ns, OutIter &&iter) const {
        if (is_vector()) {
            for (const auto &idx : idx_range) {
                *iter = fmt::format("{}{}{}{}", base, ns.bus_begin, idx, ns.bus_end);
            }
        } else {
            *iter = base;
        }
    }
};

struct name_rep;

/** Represents a list of name_rep's.
 */
struct name : x3::position_tagged {
  private:
    mutable std::optional<uint32_t> size_;

  public:
    std::vector<name_rep> rep_list;

    name();

    uint32_t size() const;

    std::string to_string(const namespace_info &ns) const;

    template <class OutIter> void append_name_bits(const namespace_info &ns, OutIter &&iter) const;
};

/** Represents a repeated name
 *
 *  Possible formats are "<*3>foo", "<*3>(a,b)", or just name_unit.
 */
struct name_rep : x3::position_tagged {
  public:
    uint32_t mult = 1;
    std::variant<name_unit, name> data;

    name_rep();

    uint32_t size() const;

    uint32_t data_size() const;

    bool is_vector() const;

    std::string to_string(const namespace_info &ns) const;

    std::vector<std::string> data_name_bits(const namespace_info &ns) const;

    template <class OutIter> void append_name_bits(const namespace_info &ns, OutIter &&iter) const {
        uint32_t n = size();
        if (n == 0)
            return;
        else if (mult == 1) {
            std::visit(
                [&ns, &iter](const auto &arg) {
                    arg.append_name_bits(ns, std::forward<OutIter>(iter));
                },
                data);
        } else {
            std::vector<std::string> cache = data_name_bits(ns);
            for (uint32_t cnt = 0; cnt < mult; ++cnt) {
                for (auto &name_bit : cache) {
                    *iter = name_bit;
                }
            }
        }
    }
};

template <class OutIter>
void name::append_name_bits(const namespace_info &ns, OutIter &&iter) const {
    for (const auto &name_rep : rep_list) {
        name_rep.append_name_bits(ns, iter);
    }
}

} // namespace ast
} // namespace spirit
} // namespace cbag

#endif // CBAG_SPIRIT_AST_H
