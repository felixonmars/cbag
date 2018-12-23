#ifndef CBAG_UTIL_SFINAE_H
#define CBAG_UTIL_SFINAE_H

#include <type_traits>

namespace cbag {
namespace util {

template <typename T> using IsInt = std::enable_if_t<std::is_integral_v<T>>;

} // namespace util
} // namespace cbag

#endif
