#ifndef CBAG_COMMON_PARAM_MAP_UTIL_H
#define CBAG_COMMON_PARAM_MAP_UTIL_H

#include <cbag/common/param_map.h>
#include <cbag/util/sfinae.h>

namespace cbag {

template <class S, typename = util::IsString<S>>
void set_param(param_map &params, S &&name,
               const std::variant<int32_t, double, bool, std::string> &val) {
    std::visit([&](auto &arg) { params.insert_or_assign(std::forward<S>(name), arg); }, val);
}

} // namespace cbag

#endif // CBAG_COMMON_DATATYPES_H
