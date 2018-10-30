
#include <cbag/common/datatypes.h>

namespace cbag {

void set_param(param_map &params, std::string &&name,
               const std::variant<int32_t, double, bool, std::string> &val) {
    std::visit([&](auto &&arg) { params[std::move(name)] = arg; }, val);
}

} // namespace cbag
