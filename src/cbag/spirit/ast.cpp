/** \file ast.cpp
 *  \brief This file implements various abstract syntax tree (AST) structures parsed by Spirit X3.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <limits>

#include <boost/functional/hash.hpp>

#include <spdlog/fmt/fmt.h>

#include <cbag/spirit/ast.h>


namespace cbag {
    namespace spirit {
        namespace ast {
            uint32_t range::size() const {
                if (step == 0)
                    return 0;
                if (stop >= start) {
                    return (stop - start + step) / step;
                } else {
                    return (start - stop + step) / step;
                }
            }

            uint32_t range::get_stop_exclude() const {
                if (stop >= start) {
                    return start + size() * step;
                } else {
                    return start - size() * step;
                }
            }

            bool range::operator==(const range &other) const {
                return start == other.start && stop == other.stop && step == other.step;
            }

            bool range::operator<(const range &other) const {
                return start < other.start
                       || (start == other.start && (stop < other.stop
                                                    || (stop == other.stop && step < other.step)));
            }

            bool name_bit::operator==(const name_bit &other) const {
                return base == other.base && index == other.index;
            }

            bool name_bit::operator<(const name_bit &other) const {
                if (base < other.base) {
                    return true;
                } else if (base == other.base) {
                    if (index) {
                        return bool(other.index) && (*index < *other.index);
                    } else {
                        return bool(other.index);
                    }
                } else {
                    return false;
                }
            }

            name_bit name_vector::operator[](uint32_t index) const {
                uint32_t range_size = idx_range.size();
                return (range_size == 0) ? name_bit(base) : name_bit(base, idx_range[index]);
            }

            bool name_vector::operator==(const name_vector &other) const {
                return base == other.base && idx_range == other.idx_range;
            }

            bool name_vector::operator<(const name_vector &other) const {
                return base < other.base || (base == other.base && (idx_range < other.idx_range));
            }

            class pre_inc_pair_first : public boost::static_visitor<> {
            public:
                template<typename T>
                void operator()(std::pair<T, T> &operand) const {
                    ++operand.first;
                }
            };

            class reset_pair_first : public boost::static_visitor<> {
            public:
                template<typename T>
                void operator()(std::pair<T, T> &operand) const {
                    operand.first.reset();
                }
            };

            class pair_equal : public boost::static_visitor<bool> {
            public:
                template<typename T>
                bool operator()(const std::pair<T, T> &operand) const {
                    return operand.first == operand.second;
                }
            };

            class eval_pair_first : public boost::static_visitor<name_bit> {
            public:
                template<typename T>
                name_bit operator()(const std::pair<T, T> &operand) const {
                    return *(operand.first);
                }
            };

            void const_list_iterator::reset() {
                list_idx = 0;
                boost::apply_visitor(reset_pair_first(), iter_pair);
            }

            const_list_iterator &const_list_iterator::operator++() {
                if (boost::apply_visitor(pair_equal(), iter_pair)) {
                    ++list_idx;
                    iter_pair = ptr->get_iter_pair(list_idx);
                } else {
                    boost::apply_visitor(pre_inc_pair_first(), iter_pair);
                }
                return *this;
            }

            bool const_list_iterator::operator!=(const const_list_iterator &other) const {
                return ptr != other.ptr || list_idx != other.list_idx
                       || iter_pair != other.iter_pair;
            }

            bool const_list_iterator::operator==(const const_list_iterator &other) const {
                return ptr == other.ptr && list_idx == other.list_idx
                       && iter_pair == other.iter_pair;
            }

            name_bit const_list_iterator::operator*() const {
                return boost::apply_visitor(eval_pair_first(), iter_pair);
            }

            name_list::const_iterator &name_list::const_iterator::operator++() {
                if (bit_index < (ptr->unit_list[unit_index]).size() - 1) {
                    ++bit_index;
                } else {
                    ++unit_index;
                    bit_index = 0;
                }

                return *this;
            }

            bool name::operator==(const name &other) const {
                unsigned long size1 = unit_list.size();
                unsigned long size2 = other.unit_list.size();

                if (size1 == size2) {
                    for (unsigned long idx = 0; idx < size1; idx++) {
                        if (unit_list[idx] != other.unit_list[idx]) {
                            return false;
                        }
                    }
                    return true;
                } else {
                    return false;
                }
            }

            bool name::operator<(const name &other) const {
                unsigned long size1 = unit_list.size();
                unsigned long size2 = other.unit_list.size();

                if (size1 < size2) {
                    return true;
                } else if (size1 == size2) {
                    for (unsigned long idx = 0; idx < size1; idx++) {
                        if (unit_list[idx] < other.unit_list[idx]) {
                            return true;
                        }
                    }
                    return false;
                } else {
                    return false;
                }
            }

        }
    }
}


namespace std {
    // define hash function for NameUnit
    template<>
    struct hash<cbag::spirit::ast::name_bit> {
        size_t operator()(const cbag::spirit::ast::name_bit &v) const {

            size_t seed = 0;
            boost::hash_combine(seed, v.base);
            boost::hash_combine(seed, (v.index) ? *(v.index) :
                                      std::numeric_limits<std::size_t>::max());

            return seed;
        }
    };

}