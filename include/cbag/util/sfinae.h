#ifndef CBAG_UTIL_SFINAE_H
#define CBAG_UTIL_SFINAE_H

#include <string>
#include <type_traits>

namespace cbag {
namespace util {

template <typename T> using IsInt = std::enable_if_t<std::is_integral_v<T>>;

template <class S>
using IsString =
    std::enable_if_t<std::is_same_v<std::string, std::remove_cv_t<std::remove_reference_t<S>>>>;

} // namespace util
} // namespace cbag

#endif
