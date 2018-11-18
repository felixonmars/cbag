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
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>

#include <cbag/spirit/variant_support.h>

namespace x3 = boost::spirit::x3;

namespace cbag {
namespace spirit {
namespace ast {

struct namespace_info {
    char bus_begin;
    char bus_end;
    char bus_delim;
    char list_delim;
    char rep_grp_begin;
    char rep_grp_end;
    std::string rep_begin;
    std::string rep_end;

    bool operator==(const namespace_info &rhs) const;
};

enum class namespace_type : uint8_t {
    CDBA = 0,
    CDL = 1,
    VERILOG = 2,
};

namespace_info get_ns_info(namespace_type ns_type);

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

    std::string to_string(const namespace_info &ns) const;
};

/** Represents a unit name; either a scalar or vector name.
 *
 *  POssible formats are "foo", "bar[2]", "baz[3:1]"
 */
struct name_unit : x3::position_tagged {
    std::string base;
    range idx_range;

    class const_iterator {
      public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::string;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

      private:
        const namespace_info *info_ = nullptr;
        const name_unit *parent_ = nullptr;
        range::const_iterator iter_;
        bool base_flag_ = true;

      public:
        const_iterator();

        const_iterator(const namespace_info *info, const name_unit *parent,
                       range::const_iterator iter, bool base_flag);

        const_iterator &operator++();
        value_type operator*() const;
        bool operator==(const const_iterator &rhs) const;
        bool operator!=(const const_iterator &rhs) const;
    };

    name_unit();

    uint32_t size() const;

    bool is_vector() const;

    const_iterator begin(const namespace_info *info) const;

    const_iterator end(const namespace_info *info) const;

    std::string to_string(const namespace_info &ns) const;
};

struct name_rep;

class const_name_rep_iterator;

using rep_vec_iter = std::vector<name_rep>::const_iterator;

/** Represents a list of name_rep's.
 */
struct name : x3::position_tagged {
  private:
    mutable std::optional<uint32_t> size_;

  public:
    std::vector<name_rep> rep_list;

    class const_iterator {
      public:
        using iterator_category = std::input_iterator_tag;
        using value_type = std::string;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

      private:
        const namespace_info *info_ = nullptr;
        rep_vec_iter vbegin_;
        rep_vec_iter vend_;
        std::unique_ptr<const_name_rep_iterator> rbegin_ = nullptr;
        std::unique_ptr<const_name_rep_iterator> rend_ = nullptr;

      public:
        const_iterator();

        const_iterator(const namespace_info *info, rep_vec_iter vbegin, rep_vec_iter vend,
                       std::unique_ptr<const_name_rep_iterator> &&rbegin,
                       std::unique_ptr<const_name_rep_iterator> &&rend);
        const_iterator(const const_iterator &rhs);
        const_iterator(const_iterator &&rhs);

        const_iterator &operator=(const const_iterator &rhs);
        const_iterator &operator=(const_iterator &&rhs);

        const_iterator &operator++();
        value_type operator*() const;
        bool operator==(const const_iterator &rhs) const;
        bool operator!=(const const_iterator &rhs) const;
    };

    name();

    uint32_t size() const;

    const_iterator begin(const namespace_info *info) const;

    const_iterator end(const namespace_info *info) const;

    std::string to_string(const namespace_info &ns) const;
};

using nu_iter_tuple =
    std::tuple<name_unit::const_iterator, name_unit::const_iterator, name_unit::const_iterator>;

using na_iter_tuple = std::tuple<name::const_iterator, name::const_iterator, name::const_iterator>;

class const_name_rep_iterator {
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::string;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

  private:
    const namespace_info *info_ = nullptr;
    uint32_t cnt_ = 0;
    std::variant<nu_iter_tuple, na_iter_tuple> iter_;

  public:
    const_name_rep_iterator();

    const_name_rep_iterator(const namespace_info *info, uint32_t cnt, nu_iter_tuple iter);

    const_name_rep_iterator(const namespace_info *info, uint32_t cnt, na_iter_tuple iter);

    const_name_rep_iterator &operator++();
    value_type operator*() const;
    bool operator==(const const_name_rep_iterator &rhs) const;
    bool operator!=(const const_name_rep_iterator &rhs) const;
};

/** Represents a repeated name
 *
 *  Possible formats are "<*3>foo", "<*3>(a,b)", or just name_unit.
 */
struct name_rep : x3::position_tagged {
    using const_iterator = const_name_rep_iterator;

    uint32_t mult = 1;
    std::variant<name_unit, name> data;

    name_rep();

    uint32_t size() const;

    bool is_vector() const;

    const_iterator begin(const namespace_info *info) const;

    const_iterator end(const namespace_info *info) const;

    std::string to_string(const namespace_info &ns) const;
};

} // namespace ast
} // namespace spirit
} // namespace cbag

#endif // CBAG_SPIRIT_AST_H
