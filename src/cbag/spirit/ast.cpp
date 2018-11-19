/** \file ast.cpp
 *  \brief This file implements various abstract syntax tree (AST) structures
 * parsed by Spirit X3.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <algorithm>

#include <cbag/spirit/ast.h>

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

uint32_t range::size() const {
    if (!size_) {
        if (step == 0) {
            size_ = 0;
        } else if (stop >= start) {
            size_ = (stop - start + step) / step;
        } else {
            size_ = (start - stop + step) / step;
        }
    }
    return *size_;
}

uint32_t range::get_stop_exclude() const {
    if (stop >= start) {
        return start + size() * step;
    } else {
        return start - size() * step;
    }
}

range::const_iterator range::begin() const { return {start, step, stop >= start}; }
range::const_iterator range::end() const { return {get_stop_exclude(), step, stop >= start}; }

uint32_t range::operator[](uint32_t index) const {
    return (stop >= start) ? start + step * index : start - step * index;
}

uint32_t range::at(uint32_t index) const {
    uint32_t n = size();
    if (index < 0 || index >= n) {
        throw std::out_of_range(fmt::format("index {} out of range (size = {})", index, n));
    }
    return operator[](index);
}

std::string range::to_string(const namespace_info &ns) const {
    if (step == 0)
        return "";
    if (start == stop)
        return fmt::format("{0}{2}{1}", ns.bus_begin, ns.bus_end, start);
    if (step == 1)
        return fmt::format("{0}{3}{2}{4}{1}", ns.bus_begin, ns.bus_end, ns.bus_delim, start, stop);
    return fmt::format("{0}{3}{2}{4}{2}{5}{1}", ns.bus_begin, ns.bus_end, ns.bus_delim, start, stop,
                       step);
}

name_unit::name_unit() = default;

uint32_t name_unit::size() const { return std::max(idx_range.size(), 1u); }

bool name_unit::is_vector() const { return idx_range.size() > 0; }

std::string name_unit::to_string(const namespace_info &ns) const {
    return base + idx_range.to_string(ns);
}

std::string name_unit::get_name_bit(const namespace_info &ns, uint32_t index) const {
    if (is_vector())
        return fmt::format("{}{}{}{}", base, ns.bus_begin, idx_range[index], ns.bus_end);
    return base;
}

name_rep::name_rep() = default;

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
    std::vector<std::string> ans;
    ans.reserve(data_size());
    auto iter = std::back_inserter(ans);
    std::visit([&ns, &iter](const auto &arg) { arg.append_name_bits(ns, iter); }, data);
    return ans;
}

name::name() = default;

uint32_t name::size() const {
    if (!size_) {
        uint32_t tot = 0;
        for (auto const &nu : rep_list) {
            tot += nu.size();
        }
        size_ = tot;
    }
    return *size_;
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
