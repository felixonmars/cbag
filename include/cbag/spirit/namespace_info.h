#ifndef CBAG_SPIRIT_NAMESPACE_INFO_H
#define CBAG_SPIRIT_NAMESPACE_INFO_H

#include <string>

namespace cbag {
namespace spirit {

struct namespace_info {
    char bus_begin;
    char bus_end;
    char bus_delim;
    char list_delim;
    char rep_grp_begin;
    char rep_grp_end;
    std::string rep_begin;
    std::string rep_end;

    bool operator==(const namespace_info &rhs) const;
};

enum class namespace_type : uint8_t {
    CDBA = 0,
    CDL = 1,
    VERILOG = 2,
};

namespace_info get_ns_info(namespace_type ns_type);

} // namespace spirit
} // namespace cbag

#endif // CBAG_SPIRIT_AST_H
