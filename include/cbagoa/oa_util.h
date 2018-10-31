#ifndef CBAGOA_OA_UTIL_H
#define CBAGOA_OA_UTIL_H

#include <cstdint>
#include <string>

#include <oa/oaDesignDB.h>

namespace spdlog {
class logger;
}

namespace cbagoa {

// constants
constexpr uint32_t LIB_ACCESS_TIMEOUT = 1;

void handle_oa_exceptions(spdlog::logger &logger);

oa::oaLib *open_library_read(const oa::oaCdbaNS &ns, const std::string &lib_name);

} // namespace cbagoa

#endif
