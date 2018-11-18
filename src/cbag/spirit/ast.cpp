/** \file ast.cpp
 *  \brief This file implements various abstract syntax tree (AST) structures
 * parsed by Spirit X3.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <algorithm>

#include <fmt/core.h>

#include <cbag/spirit/ast.h>
#include <cbag/util/overload.h>

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

range::const_iterator range::begin() const { return {start, step, stop > start}; }
range::const_iterator range::end() const { return {get_stop_exclude(), step, stop > start}; }

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

name_unit::const_iterator::const_iterator() = default;

name_unit::const_iterator::const_iterator(const namespace_info *info, const name_unit *parent,
                                          range::const_iterator iter, bool base_flag)
    : info_(info), parent_(parent), iter_(std::move(iter)), base_flag_(base_flag) {}

name_unit::const_iterator &name_unit::const_iterator::operator++() {
    if (base_flag_)
        ++iter_;
    else
        base_flag_ = true;
    return *this;
}

name_unit::const_iterator::value_type name_unit::const_iterator::operator*() const {
    if (base_flag_)
        return fmt::format("{}{}{}{}", parent_->base, info_->bus_begin, *iter_, info_->bus_end);
    return parent_->base;
}

bool name_unit::const_iterator::operator==(const name_unit::const_iterator &rhs) const {
    return info_ == rhs.info_ && parent_ == rhs.parent_ && iter_ == rhs.iter_ &&
           base_flag_ == rhs.base_flag_;
}

bool name_unit::const_iterator::operator!=(const name_unit::const_iterator &rhs) const {
    return !(*this == rhs);
}

name_unit::name_unit() = default;

uint32_t name_unit::size() const { return std::max(idx_range.size(), 1u); }

bool name_unit::is_vector() const { return idx_range.size() > 0; }

name_unit::const_iterator name_unit::begin(const namespace_info *info) const {
    return {info, this, idx_range.begin(), is_vector()};
}

name_unit::const_iterator name_unit::end(const namespace_info *info) const {
    return {info, this, idx_range.end(), true};
}

std::string name_unit::to_string(const namespace_info &ns) const {
    return base + idx_range.to_string(ns);
}

const_name_rep_iterator::const_name_rep_iterator() = default;

const_name_rep_iterator::const_name_rep_iterator(const namespace_info *info, uint32_t cnt,
                                                 nu_iter_tuple iter)
    : info_(info), cnt_(cnt), iter_(std::move(iter)) {}

const_name_rep_iterator::const_name_rep_iterator(const namespace_info *info, uint32_t cnt,
                                                 na_iter_tuple iter)
    : info_(info), cnt_(cnt), iter_(std::move(iter)) {}

const_name_rep_iterator &const_name_rep_iterator::operator++() {
    std::visit(
        [this](auto &arg) {
            auto &ans = ++(std::get<2>(arg));
            if (ans == std::get<1>(arg)) {
                std::get<2>(arg) = std::get<0>(arg);
                ++cnt_;
            }
        },
        iter_);

    return *this;
}

const_name_rep_iterator::value_type const_name_rep_iterator::operator*() const {
    return std::visit([](auto &arg) { return *(std::get<2>(arg)); }, iter_);
}

bool const_name_rep_iterator::operator==(const const_name_rep_iterator &rhs) const {
    return info_ == rhs.info_ && cnt_ == rhs.cnt_ && iter_ == rhs.iter_;
}

bool const_name_rep_iterator::operator!=(const const_name_rep_iterator &rhs) const {
    return !(*this == rhs);
}

name_rep::name_rep() = default;

uint32_t name_rep::size() const {
    return mult * std::visit([](const auto &arg) { return arg.size(); }, data);
}

bool name_rep::is_vector() const {
    const name_unit *ptr = std::get_if<name_unit>(&data);
    return ptr && ptr->is_vector();
}

const_name_rep_iterator iter_helper(const namespace_info *info,
                                    const std::variant<name_unit, name> &data, uint32_t cnt) {
    return std::visit(
        overload{
            [&info, &cnt](const name_unit &arg) {
                return const_name_rep_iterator(
                    info, cnt, nu_iter_tuple(arg.begin(info), arg.end(info), arg.begin(info)));
            },
            [&info, &cnt](const name &arg) {
                return const_name_rep_iterator(
                    info, cnt, na_iter_tuple(arg.begin(info), arg.end(info), arg.begin(info)));
            },
        },
        data);
}

const_name_rep_iterator name_rep::begin(const namespace_info *info) const {
    // the iterator logic now will not work if name_rep repeats an empty name
    // in this special case, setting initial cnt to mult solves the issue
    uint32_t cnt = (size() > 0) ? 0 : mult;
    return iter_helper(info, data, cnt);
}

const_name_rep_iterator name_rep::end(const namespace_info *info) const {
    return iter_helper(info, data, mult);
} // namespace ast

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

name::const_iterator::const_iterator() = default;

name::const_iterator::const_iterator(const namespace_info *info, rep_vec_iter vbegin,
                                     rep_vec_iter vend,
                                     std::unique_ptr<const_name_rep_iterator> &&rbegin,
                                     std::unique_ptr<const_name_rep_iterator> &&rend)
    : info_(info), vbegin_(std::move(vbegin)), vend_(std::move(vend)), rbegin_(std::move(rbegin)),
      rend_(std::move(rend)) {}

name::const_iterator::const_iterator(const const_iterator &rhs) { *this = rhs; }

name::const_iterator::const_iterator(const_iterator &&rhs) { *this = std::move(rhs); }

name::const_iterator &name::const_iterator::operator=(const name::const_iterator &rhs) {
    info_ = rhs.info_;
    vbegin_ = rhs.vbegin_;
    vend_ = rhs.vend_;
    if (rhs.rbegin_ == nullptr)
        rbegin_.reset();
    else
        rbegin_ = std::make_unique<const_name_rep_iterator>(*rhs.rbegin_);
    if (rhs.rend_ == nullptr)
        rend_.reset();
    else
        rend_ = std::make_unique<const_name_rep_iterator>(*rhs.rend_);

    return *this;
}

name::const_iterator &name::const_iterator::operator=(name::const_iterator &&rhs) {
    info_ = std::move(rhs.info_);
    vbegin_ = std::move(rhs.vbegin_);
    vend_ = std::move(rhs.vend_);
    rbegin_ = std::move(rhs.rbegin_);
    rend_ = std::move(rhs.rend_);
    return *this;
}

name::const_iterator &name::const_iterator::operator++() {
    ++(*rbegin_);
    if (*rbegin_ == *rend_) {
        ++vbegin_;
        if (vbegin_ != vend_) {
            rbegin_ = std::make_unique<const_name_rep_iterator>(vbegin_->begin(info_));
            rend_ = std::make_unique<const_name_rep_iterator>(vbegin_->end(info_));
        }
    }
    return *this;
}

name::const_iterator::value_type name::const_iterator::operator*() const { return *(*rbegin_); }

bool ptr_equal(const_name_rep_iterator *p1, const_name_rep_iterator *p2) {
    return (p1 == nullptr && p2 == nullptr) || (p1 != nullptr && p2 != nullptr && *p1 == *p2);
}

bool name::const_iterator::operator==(const name::const_iterator &other) const {
    return info_ == other.info_ && vbegin_ == other.vbegin_ && vend_ == other.vend_ &&
           ptr_equal(rbegin_.get(), other.rbegin_.get()) &&
           ptr_equal(rend_.get(), other.rend_.get());
}

bool name::const_iterator::operator!=(const name::const_iterator &other) const {
    return !(*this == other);
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

name::const_iterator name::begin(const namespace_info *info) const {
    rep_vec_iter vbegin = rep_list.begin();
    rep_vec_iter vend = rep_list.end();
    if (vbegin == vend) {
        return {info, std::move(vbegin), std::move(vend), nullptr, nullptr};
    }
    auto rbegin = std::make_unique<const_name_rep_iterator>(vbegin->begin(info));
    auto rend = std::make_unique<const_name_rep_iterator>(vbegin->end(info));

    return {info, std::move(vbegin), std::move(vend), std::move(rbegin), std::move(rend)};
}

name::const_iterator name::end(const namespace_info *info) const {
    if (rep_list.empty()) {
        return {info, rep_list.end(), rep_list.end(), nullptr, nullptr};
    }
    auto &last = rep_list.back();
    auto rbegin = std::make_unique<const_name_rep_iterator>(last.end(info));
    auto rend = std::make_unique<const_name_rep_iterator>(last.end(info));
    return {info, rep_list.end(), rep_list.end(), std::move(rbegin), std::move(rend)};
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
