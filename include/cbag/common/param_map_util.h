#ifndef CBAG_COMMON_PARAM_MAP_UTIL_H
#define CBAG_COMMON_PARAM_MAP_UTIL_H

#include <cbag/common/param_map.h>
#include <cbag/util/sfinae.h>

namespace cbag {

template <class S, typename = util::IsString<S>>
void set_param(param_map &params, S &&name, const param_t &val) {
    std::visit([&](auto &arg) { params.insert_or_assign(std::forward<S>(name), arg); }, val);
}

} // namespace cbag

#endif
