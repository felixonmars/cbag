
#ifndef CBAG_SPIRIT_UTIL_H
#define CBAG_SPIRIT_UTIL_H

#include <fmt/core.h>

#include <cbag/spirit/ast.h>
#include <cbag/spirit/namespace_info.h>

namespace cbag {
namespace spirit {
namespace util {

using name = ast::name;
using name_unit = ast::name_unit;
using name_rep = ast::name_rep;
using range = ast::range;

template <typename NS>
using IsNameSpace =
    std::enable_if_t<std::is_same_v<std::remove_reference_t<NS>, spirit::namespace_cdba> ||
                         std::is_same_v<std::remove_reference_t<NS>, spirit::namespace_cdba>,
                     int>;

template <class OutIter>
void get_name_bits_helper(const name_unit &obj, OutIter &&iter, const char *fmt_str) {
    if (obj.is_vector()) {
        for (const auto &idx : obj.idx_range) {
            *iter = fmt::format(fmt_str, obj.base, idx);
        }
    } else {
        *iter = obj.base;
    }
}

template <class OutIter> void get_name_bits(const name_unit &obj, OutIter &&iter, namespace_cdba) {
    get_name_bits_helper(obj, std::forward<OutIter>(iter), "{}<{}>");
}

template <class OutIter>
void get_name_bits(const name_unit &obj, OutIter &&iter, namespace_verilog) {
    get_name_bits_helper(obj, std::forward<OutIter>(iter), "{}[{}]");
}

template <class OutIter, class NS, typename = IsNameSpace<NS>>
void get_name_bits(const name &obj, OutIter &&iter, NS ns);

template <class OutIter, class NS, typename = IsNameSpace<NS>>
void get_name_bits(const name_rep &obj, OutIter &&iter, NS ns) {
    uint32_t n = obj.size();
    if (n == 0)
        return;
    else if (obj.mult == 1) {
        std::visit(
            [&ns, &iter](const auto &arg) { get_name_bits(arg, std::forward<OutIter>(iter), ns); },
            obj.data);
    } else {
        std::vector<std::string> cache = std::visit(
            [&ns](const auto &arg) {
                std::vector<std::string> ans;
                ans.reserve(arg.size());
                util::get_name_bits(arg, std::back_inserter(ans), ns);
                return ans;
            },
            obj.data);
        for (uint32_t cnt = 0; cnt < obj.mult; ++cnt) {
            for (auto &name_bit : cache) {
                *iter = name_bit;
            }
        }
    }
}

template <class OutIter, class NS, typename>
void get_name_bits(const name &obj, OutIter &&iter, NS ns) {
    for (const auto &name_rep : obj.rep_list) {
        get_name_bits(name_rep, iter, ns);
    }
}

template <class OutIter>
void get_partition_helper(OutIter &&iter, const name_unit &obj, std::vector<name_rep> &rep_list,
                          uint32_t &cum_size, uint32_t chunk) {
    uint32_t cur_size = obj.idx_range.size();
    if (cur_size > 0) {
        // this is a vector name unit
        uint32_t step = obj.idx_range.step;
        uint32_t start_idx = 0;
        // this vector name could be very long, use for loop to partition
        // into chunks
        for (uint32_t stop_idx = chunk - cum_size; stop_idx <= cur_size;
             start_idx = stop_idx, stop_idx += chunk) {
            rep_list.emplace_back(1, name_unit(obj.base, {obj.idx_range[start_idx],
                                                          obj.idx_range[stop_idx - 1], step}));
            *iter = name(std::move(rep_list));
            rep_list.clear();
        }
        // check for residual chunks, and set cum_size properly
        if (start_idx < cur_size) {
            rep_list.emplace_back(
                1, name_unit(obj.base, {obj.idx_range[start_idx], obj.idx_range.stop, step}));
            cum_size = cur_size - start_idx;
        } else {
            cum_size = 0;
        }
    } else {
        // this is a scalar name unit, just add itself
        rep_list.emplace_back(1, obj);
        ++cum_size;
        if (cum_size == chunk) {
            *iter = name(std::move(rep_list));
            rep_list.clear();
            cum_size = 0;
        }
    }
}

template <class OutIter>
void get_partition_helper(OutIter &&iter, const name &obj, std::vector<name_rep> &rep_list,
                          uint32_t &cum_size, uint32_t chunk);

template <class OutIter>
void get_partition_helper(OutIter &&iter, const name_rep &obj, std::vector<name_rep> &rep_list,
                          uint32_t &cum_size, uint32_t chunk) {
    uint32_t data_n = obj.data_size();

    uint32_t remain_rep = obj.mult;
    while (remain_rep > 0) {
        std::ldiv_t result =
            std::div(static_cast<long>(chunk - cum_size), static_cast<long>(data_n));
        uint32_t mult1 = static_cast<uint32_t>(result.quot);
        if (mult1 >= remain_rep) {
            // this name_rep is used up
            rep_list.emplace_back(remain_rep, obj.data);
            cum_size += remain_rep * data_n;
            remain_rep = 0;
        } else {
            // still has some remaining
            if (mult1 > 0) {
                rep_list.emplace_back(mult1, obj.data);
                cum_size += mult1 * data_n;
            }
            remain_rep -= mult1;
            if (result.rem != 0) {
                // we need to partition the data
                std::visit(
                    [&iter, &rep_list, &cum_size, &chunk](const auto &v) {
                        get_partition_helper(iter, v, rep_list, cum_size, chunk);
                    },
                    obj.data);
                remain_rep -= 1;
            }
        }
        if (cum_size == chunk) {
            // assign to output iterator
            *iter = name(std::move(rep_list));
            rep_list.clear();
            cum_size = 0;
        }
    }
}

template <class OutIter>
void get_partition_helper(OutIter &&iter, const name &obj, std::vector<name_rep> &rep_list,
                          uint32_t &cum_size, uint32_t chunk) {
    for (const auto &nr : obj.rep_list) {
        get_partition_helper(iter, nr, rep_list, cum_size, chunk);
    }
}

template <class OutIter> void get_partition(const name &obj, uint32_t chunk, OutIter &&iter) {
    std::vector<name_rep> temp;
    uint32_t cum_size = 0;
    get_partition_helper(std::forward<OutIter>(iter), obj, temp, cum_size, chunk);
    if (cum_size != 0) {
        *iter = name(std::move(temp));
    }
}

} // namespace util
} // namespace spirit
} // namespace cbag

#endif // CBAG_SPIRIT_AST_H
