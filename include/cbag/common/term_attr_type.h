
#ifndef CBAG_COMMON_TERM_ATTR_TYPE_H
#define CBAG_COMMON_TERM_ATTR_TYPE_H

#include <cstdint>

namespace cbag {

enum class term_attr_type : uint8_t { name = 0, hasPins = 1, numBits = 2 };

} // namespace cbag

#endif
