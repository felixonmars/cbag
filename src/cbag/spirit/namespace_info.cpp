
#include <fmt/core.h>

#include <cbag/spirit/namespace_info.h>

namespace cbag {
namespace spirit {
namespace ast {

bool namespace_info::operator==(const namespace_info &rhs) const {
    return (bus_begin == rhs.bus_begin && bus_end == rhs.bus_end && bus_delim == rhs.bus_delim &&
            list_delim == rhs.list_delim && rep_grp_begin == rhs.rep_grp_begin &&
            rep_grp_end == rhs.rep_grp_end && rep_begin == rhs.rep_begin && rep_end == rhs.rep_end);
}

namespace_info get_ns_info(namespace_type ns_type) {
    switch (ns_type) {
    case namespace_type::CDBA:
        return {'<', '>', ':', ',', '(', ')', "<*", ">"};
    case namespace_type::CDL:
        return {'<', '>', ':', ',', '(', ')', "", ""};
    case namespace_type::VERILOG:
        return {'[', ']', ':', ',', '(', ')', "", ""};
    default:
        throw std::invalid_argument(
            fmt::format("Unsupported namespace type: {}", static_cast<uint8_t>(ns_type)));
    }
}

} // namespace ast
} // namespace spirit
} // namespace cbag
