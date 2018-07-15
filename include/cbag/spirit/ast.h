/** \file ast.h
 *  \brief This file defines various abstract syntax tree (AST) structures parsed by Spirit X3.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_SPIRIT_AST_H
#define CBAG_SPIRIT_AST_H

#include <algorithm>
#include <vector>

#include <boost/optional/optional.hpp>
#include <boost/variant.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/variant.hpp>
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
                range()
                        : start(0), stop(0), step(0) {}

                range(uint32_t start, uint32_t stop, uint32_t step)
                        : start(start), stop(stop), step(step) {}

                uint32_t size() const;

                uint32_t get_stop_exclude() const;

                inline uint32_t operator[](uint32_t index) const {
                    return (stop >= start) ? start + step * index : start - step * index;
                }

                bool operator==(const range &other) const;

                inline bool operator!=(const range &other) const { return !(*this == other); }

                bool operator<(const range &other) const;

                // boost serialization
                template<class Archive>
                void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
                    ar & BOOST_SERIALIZATION_NVP(start);
                    ar & BOOST_SERIALIZATION_NVP(stop);
                    ar & BOOST_SERIALIZATION_NVP(step);
#pragma clang diagnostic pop
                }

                uint32_t start;
                uint32_t stop;
                uint32_t step;
            };

            /** Represents a single name
             *
             *  Could be either a scalar name ("foo") or a vector bit name ("bar<3>").
             */
            struct name_bit : x3::position_tagged {

                name_bit()
                        : base("") {}

                explicit name_bit(std::string base) : base(std::move(base)) {}

                name_bit(std::string base, uint32_t index) : base(std::move(base)), index(index) {}

                bool operator==(const name_bit &other) const;

                inline bool operator!=(const name_bit &other) const { return !(*this == other); }

                bool operator<(const name_bit &other) const;

                // boost serialization
                template<class Archive>
                void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
                    ar & BOOST_SERIALIZATION_NVP(base);
                    ar & BOOST_SERIALIZATION_NVP(index);
#pragma clang diagnostic pop
                }

                std::string base;
                boost::optional<uint32_t> index;
            };

            /** Represents a name group (scalar or vector) with the same base name
             *
             *  Possible formats are "foo", "bar<3:0>", or "baz<1>".
             */
            struct name_vector : x3::position_tagged {
                name_vector()
                        : base(""), idx_range({0, 0, 0}) {}

                inline uint32_t size() const { return std::max(idx_range.size(), 1u); }

                inline bool is_vector() { return idx_range.size() > 0; }

                inline const_iterator begin() const { return {this, 0}; }

                inline const_iterator end() const { return {this, size()}; }

                name_bit operator[](uint32_t index) const;

                bool operator==(const name_vector &other) const;

                inline bool operator!=(const name_vector &other) const { return !(*this == other); }

                bool operator<(const name_vector &other) const;

                // boost serialization
                template<class Archive>
                void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
                    ar & BOOST_SERIALIZATION_NVP(base);
                    ar & BOOST_SERIALIZATION_NVP(idx_range);
#pragma clang diagnostic pop
                }

                std::string base;
                range idx_range;
            };

            class const_vector_iterator {
            public:
                const_vector_iterator(const name_vector *ptr, uint32_t bit_index)
                        : ptr(ptr), bit_index(bit_index) {}

                inline void reset() { bit_index = 0; }

                inline const_vector_iterator &operator++() {
                    ++bit_index;
                    return *this;
                }

                inline bool operator!=(const const_vector_iterator &other) const {
                    return ptr != other.ptr || bit_index != other.bit_index;
                }

                inline bool operator==(const const_vector_iterator &other) const {
                    return ptr == other.ptr && bit_index == other.bit_index;
                }

                inline name_bit operator*() const { return (*ptr)[bit_index]; }

            private:
                const name_vector *ptr;
                uint32_t bit_index;
            };

            // forward declaration of name_group and name
            struct name_list;
            struct const_group_iterator;

            using const_item_iter_pair = boost::variant<
                    std::pair<const_vector_iterator, const_vector_iterator>,
                    std::pair<const_group_iterator, const_group_iterator>>;

            class const_list_iterator {
            public:
                const_list_iterator(const name_list *ptr, uint32_t list_idx,
                                    const const_item_iter_pair &iter_pair)
                        : ptr(ptr), list_idx(list_idx), iter_pair(iter_pair) {}

                const_list_iterator(const name_list *ptr, uint32_t list_idx,
                                    const_item_iter_pair &&iter_pair)
                        : ptr(ptr), list_idx(list_idx), iter_pair(iter_pair) {}

                void reset();

                const_list_iterator &operator++();

                bool operator!=(const const_list_iterator &other) const;

                bool operator==(const const_list_iterator &other) const;

                name_bit operator*() const;

            private:
                const name_list *ptr;
                uint32_t list_idx;
                const_item_iter_pair iter_pair;
            };

            class const_group_iterator {
            public:
                const_group_iterator(const name_list *ptr, uint32_t list_idx,
                                    const const_item_iter_pair &iter_pair)
                        : ptr(ptr), list_idx(list_idx), iter_pair(iter_pair) {}

                const_list_iterator(const name_list *ptr, uint32_t list_idx,
                                    const_item_iter_pair &&iter_pair)
                        : ptr(ptr), list_idx(list_idx), iter_pair(iter_pair) {}

                void reset();

                const_list_iterator &operator++();

                bool operator!=(const const_list_iterator &other) const;

                bool operator==(const const_list_iterator &other) const;

                name_bit operator*() const;

            private:
                const name_list *ptr;
                uint32_t list_idx;
                const_item_iter_pair iter_pair;
            };

            /** Represents a list of names
             *
             *  Possible formats are "<*2>foo", "<*3>bar<3:0>", or "<*2>(baz,cat)".
             */
            struct name_list : x3::position_tagged {
                class const_iterator {
                public:
                    const_iterator(const name_list *ptr, unsigned long unit_index,
                                   uint32_t bit_index)
                            : ptr(ptr), unit_index(unit_index), bit_index(bit_index) {}

                    const_iterator &operator++();

                    inline bool operator!=(const const_iterator &other) {
                        return ptr != other.ptr || unit_index != other.unit_index ||
                               bit_index != other.bit_index;
                    }

                    inline bool operator==(const const_iterator &other) {
                        return ptr == other.ptr && unit_index == other.unit_index &&
                               bit_index == other.bit_index;
                    }

                    inline name_bit
                    operator*() const { return ptr->unit_list[unit_index][bit_index]; }

                private:
                    const name *ptr;
                    unsigned long unit_index;
                    uint32_t bit_index;
                };


                name_list()
                        : names() {}

                inline const_iterator begin() const { return {this, 0, 0}; }

                inline const_iterator end() const { return {this, unit_list.size(), 0}; }

                bool operator==(const name_list &other) const;

                inline bool operator!=(const name_list &other) const { return !(*this == other); }

                bool operator<(const name_list &other) const;

                // boost serialization
                template<class Archive>
                void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
                    ar & BOOST_SERIALIZATION_NVP(names);
#pragma clang diagnostic pop
                }

                std::vector<boost::variant<name_vector, name_group>> names;
            };

            /** Represents a list of name units.
             */
            struct name : x3::position_tagged {

                bool operator==(const name &other) const;

                inline bool operator!=(const name &other) const { return !(*this == other); }

                bool operator<(const name &other) const;

                // boost serialization
                template<class Archive>
                void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
                    ar & BOOST_SERIALIZATION_NVP(value);
#pragma clang diagnostic pop
                }

                boost::variant<name_vector, name_group, name_list> value;
            };

            /** Represents a repeated group of names
             *
             *  Possible formats are "<*2>foo", "<*3>bar<3:0>", or "<*2>(baz,cat)".
             */
            struct name_group : x3::position_tagged {

                name_group()
                        : mult(1), group() {}

                bool operator==(const name_group &other) const;

                inline bool operator!=(const name_group &other) const { return !(*this == other); }

                bool operator<(const name_group &other) const;

                // boost serialization
                template<class Archive>
                void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
                    ar & BOOST_SERIALIZATION_NVP(mult);
                    ar & BOOST_SERIALIZATION_NVP(group);
#pragma clang diagnostic pop
                }

                uint32_t mult;
                name group;
            };
        }
    }
}


#endif //CBAG_SPIRIT_AST_H
