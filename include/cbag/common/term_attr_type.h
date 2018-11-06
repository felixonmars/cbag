
#ifndef CBAG_COMMON_TERM_ATTR_TYPE_H
#define CBAG_COMMON_TERM_ATTR_TYPE_H

#include <cstdint>

namespace cbag {

enum class term_attr_type : uint8_t { name, hasPins, numBits };
constexpr term_attr_type tatName = term_attr_type::name;
constexpr term_attr_type tatHasPin = term_attr_type::hasPins;
constexpr term_attr_type tatNumBits = term_attr_type::numBits;

} // namespace cbag

#endif
