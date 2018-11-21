#ifndef CBAG_SPIRIT_NAMESPACE_INFO_H
#define CBAG_SPIRIT_NAMESPACE_INFO_H

#include <string>

namespace cbag {
namespace spirit {

struct namespace_cdba {};
struct namespace_verilog {};

enum class namespace_type : uint8_t {
    CDBA = 0,
    CDL = 1,
    VERILOG = 2,
};

} // namespace spirit
} // namespace cbag

#endif // CBAG_SPIRIT_AST_H
