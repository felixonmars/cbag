//
// Created by erichang on 7/4/18.
//

#ifndef CBAG_SPIRIT_AST_H
#define CBAG_SPIRIT_AST_H

#include <algorithm>
#include <vector>

#include <boost/optional/optional.hpp>
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

                std::string to_string() const;

                inline uint32_t operator[](uint32_t index) const {
                    return (stop >= start) ? start + step * index : start - step * index;
                }

                bool operator==(const range &other) const;

                inline bool operator!=(const range &other) const { return !(*this == other); }

                bool operator<(const range &other) const;

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

                std::string to_string() const;

                bool operator==(const name_bit &other) const;

                inline bool operator!=(const name_bit &other) const { return !(*this == other); }

                bool operator<(const name_bit &other) const;

                std::string base;
                boost::optional<uint32_t> index;
            };

            /** Represents a name unit
             *
             *  Possible formats are "foo", "bar<3:0>", "<*3>baz", or "<*3>asdf<3:0>".
             */
            struct name_unit : x3::position_tagged {

                name_unit()
                        : mult(1), base(""), idx_range({0, 0, 0}) {}

                inline uint32_t size() const { return mult * std::max(idx_range.size(), 1u); }

                inline bool is_vector() { return idx_range.size() > 0; }

                std::string to_string() const;

                name_bit operator[](uint32_t index) const;

                bool operator==(const name_unit &other) const;

                inline bool operator!=(const name_unit &other) const { return !(*this == other); }

                bool operator<(const name_unit &other) const;

                uint32_t mult;
                std::string base;
                range idx_range;
            };

            /** Represents a list of name units.
             */
            struct name : x3::position_tagged {
                class const_iterator {
                public:
                    const_iterator(const name *ptr, unsigned long unit_index, uint32_t bit_index)
                            : ptr(ptr), unit_index(unit_index), bit_index(bit_index) {}

                    const_iterator &operator++();

                    inline bool operator!=(const const_iterator &other) {
                        return ptr != other.ptr || unit_index != other.unit_index || bit_index != other.bit_index;
                    }

                    inline name_bit operator*() const { return ptr->unit_list[unit_index][bit_index]; }

                private:
                    const name *ptr;
                    unsigned long unit_index;
                    uint32_t bit_index;
                };

                inline const_iterator begin() const { return {this, 0, 0}; }

                inline const_iterator end() const { return {this, unit_list.size(), 0}; }

                bool operator==(const name &other) const;

                inline bool operator!=(const name &other) const { return !(*this == other); }

                bool operator<(const name &other) const;

                std::vector<name_unit> unit_list;
            };
        }
    }
}


#endif //CBAG_SPIRIT_AST_H
