/** \file ast.cpp
 *  \brief This file implements various abstract syntax tree (AST) structures
 * parsed by Spirit X3.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <algorithm>
#include <cassert>

#include <fmt/core.h>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/namespace_info.h>
#include <cbag/spirit/util.h>

namespace cbag {
namespace spirit {
namespace ast {

range::const_iterator::const_iterator() = default;

range::const_iterator::const_iterator(uint32_t val, uint32_t step, bool up)
    : val_(val), step_(step), up_(up) {}

range::const_iterator &range::const_iterator::operator++() {
    if (up_)
        val_ += step_;
    else
        val_ -= step_;
    return *this;
}

range::const_iterator::reference range::const_iterator::operator*() const { return val_; }

bool range::const_iterator::operator==(const range::const_iterator &rhs) const {
    return val_ == rhs.val_ && step_ == rhs.step_ && up_ == rhs.up_;
}

bool range::const_iterator::operator!=(const range::const_iterator &rhs) const {
    return !(*this == rhs);
}

range::range() {}

range::range(uint32_t start, uint32_t stop, uint32_t step) : start(start), stop(stop), step(step) {}

bool range::empty() const { return step == 0; }

uint32_t range::size() const {
    if (step == 0) {
        return 0;
    } else if (stop >= start) {
        return (stop - start + step) / step;
    } else {
        return (start - stop + step) / step;
    }
}

uint32_t range::get_stop_exclude() const { return operator[](size()); }

range::const_iterator range::begin() const { return {start, step, stop >= start}; }
range::const_iterator range::end() const { return {get_stop_exclude(), step, stop >= start}; }

uint32_t range::operator[](uint32_t index) const {
    return (stop >= start) ? start + step * index : start - step * index;
}

std::string range::to_string(const namespace_info &ns, bool show_stop) const {
    if (step == 0)
        return "";
    if (start == stop && !show_stop)
        return fmt::format("{0}{2}{1}", ns.bus_begin, ns.bus_end, start);
    if (start == stop || step == 1)
        return fmt::format("{0}{3}{2}{4}{1}", ns.bus_begin, ns.bus_end, ns.bus_delim, start, stop);
    return fmt::format("{0}{3}{2}{4}{2}{5}{1}", ns.bus_begin, ns.bus_end, ns.bus_delim, start, stop,
                       step);
}

name_unit::name_unit() = default;

name_unit::name_unit(std::string base, range idx_range)
    : base(std::move(base)), idx_range(std::move(idx_range)) {}

bool name_unit::empty() const { return base.empty(); }

uint32_t name_unit::size() const { return std::max(idx_range.size(), 1u); }

bool name_unit::is_vector() const { return !idx_range.empty(); }

std::string name_unit::to_string(const namespace_info &ns) const {
    return base + idx_range.to_string(ns);
}

std::string name_unit::get_name_bit(const namespace_info &ns, uint32_t index) const {
    assert(0 <= index && index < size());
    if (is_vector())
        return fmt::format("{}{}{}{}", base, ns.bus_begin, idx_range[index], ns.bus_end);
    return base;
}

name_rep::name_rep() = default;

name_rep::name_rep(uint32_t mult, std::variant<name_unit, name> data)
    : mult(mult), data(std::move(data)) {}

name_rep::name_rep(uint32_t mult, name_unit nu) : mult(mult), data(std::move(nu)) {}

bool name_rep::empty() const { return mult == 0; }

uint32_t name_rep::size() const { return mult * data_size(); }

uint32_t name_rep::data_size() const {
    return std::visit([](const auto &arg) { return arg.size(); }, data);
}

bool name_rep::is_vector() const {
    const name_unit *ptr = std::get_if<name_unit>(&data);
    return ptr && ptr->is_vector();
}

std::string name_rep::to_string(const namespace_info &ns) const {
    if (mult == 0)
        return "";
    std::string base = std::visit([&ns](const auto &arg) { return arg.to_string(ns); }, data);
    if (mult == 1)
        return base;

    if (ns.rep_begin.empty()) {
        // handle namespaces that do not support name repetition
        std::string ans = base;
        ans.reserve(mult * base.size() + (mult - 1));
        for (uint32_t idx = 1; idx < mult; ++idx) {
            ans.append(1, ns.list_delim);
            ans.append(base);
        }
        return ans;
    }
    if (std::holds_alternative<name_unit>(data))
        return fmt::format("{0}{2}{1}{3}", ns.rep_begin, ns.rep_end, mult, base);
    return fmt::format("{0}{4}{1}{2}{5}{3}", ns.rep_begin, ns.rep_end, ns.rep_grp_begin,
                       ns.rep_grp_end, mult, base);
}

std::vector<std::string> name_rep::data_name_bits(const namespace_info &ns) const {
    return std::visit(
        [&ns](const auto &arg) {
            std::vector<std::string> ans;
            ans.reserve(arg.size());
            util::get_name_bits(arg, ns, std::back_inserter(ans));
            return ans;
        },
        data);
}

name::name() = default;

name::name(std::vector<name_rep> rep_list) : rep_list(std::move(rep_list)) {}

bool name::empty() const { return rep_list.empty(); }

uint32_t name::size() const {
    uint32_t tot = 0;
    for (auto const &nr : rep_list) {
        tot += nr.size();
    }
    return tot;
}

std::string name::to_string(const namespace_info &ns) const {
    std::size_t n = rep_list.size();
    if (n == 0)
        return "";
    std::string ans = rep_list.front().to_string(ns);
    for (std::size_t idx = 1; idx < n; ++idx) {
        ans.append(1, ns.list_delim);
        ans.append(rep_list[idx].to_string(ns));
    }
    return ans;
}

} // namespace ast
} // namespace spirit
} // namespace cbag
