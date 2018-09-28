#ifndef CBAG_UTIL_OVERLOAD_H
#define CBAG_UTIL_OVERLOAD_H

#include <optional>
#include <type_traits>

namespace cbag {

template <class... Ts> struct overload : Ts... { using Ts::operator()...; };
template <class... Ts> overload(Ts...)->overload<Ts...>;

} // namespace cbag

#endif
