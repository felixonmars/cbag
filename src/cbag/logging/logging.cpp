#include <cbag/logging/logging.h>

#include "spdlog/details/signal_handler.h"
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace cbag {

constexpr uint32_t max_log_size = 1024 * 1024 * 10;
constexpr uint32_t num_log_file = 3;

void init_logging() {
    if (spdlog::get("bag") == nullptr) {
        spdlog::installCrashHandler();

        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_st>(
            "bag.log", max_log_size, num_log_file));
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
        stdout_sink->set_level(spdlog::level::warn);
        sinks.push_back(std::move(stdout_sink));

        auto logger = std::make_shared<spdlog::logger>("bag", sinks.begin(), sinks.end());
        spdlog::register_logger(logger);

        auto cbag_logger = logger->clone("cbag");
        spdlog::register_logger(cbag_logger);

        spdlog::flush_on(spdlog::level::warn);
    }
}

std::shared_ptr<spdlog::logger> get_bag_logger() {
    init_logging();
    return spdlog::get("bag");
}

std::shared_ptr<spdlog::logger> get_cbag_logger() {
    init_logging();
    return spdlog::get("cbag");
}

} // namespace cbag
