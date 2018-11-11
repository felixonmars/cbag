#ifndef CBAG_LOGGING_LOGGING_H
#define CBAG_LOGGING_LOGGING_H

#include <memory>

#include <cbag/logging/spdlog.h>

namespace cbag {

void init_logging();

std::shared_ptr<spdlog::logger> get_bag_logger();

std::shared_ptr<spdlog::logger> get_cbag_logger();

} // namespace cbag

#endif
