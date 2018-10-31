
#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include <oa/oaDesignDB.h>

#include <cbagoa/oa_util.h>

namespace cbagoa {

void handle_oa_exceptions(spdlog::logger &logger) {
    logger.error("Exception caught, exiting");
    try {
        throw;
    } catch (oa::oaCompatibilityError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA Compatibility Error: " + msg_std);
    } catch (oa::oaDMError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA DM Error: " + msg_std);
    } catch (oa::oaError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA Error: " + msg_std);
    } catch (oa::oaDesignError &ex) {
        throw std::runtime_error("OA Design Error: " + std::string(ex.getMsg()));
    }
}

// write all symbol views to file
// get library read access
oa::oaLib *open_library_read(const oa::oaCdbaNS &ns, const std::string &lib_name) {
    oa::oaLib *lib_ptr = oa::oaLib::find(oa::oaScalarName(ns, lib_name.c_str()));
    if (lib_ptr == nullptr) {
        throw std::invalid_argument(fmt::format("Cannot find library {}", lib_name));
    }
    if (!lib_ptr->getAccess(oa::oacReadLibAccess, LIB_ACCESS_TIMEOUT)) {
        throw std::runtime_error(fmt::format("Cannot obtain read access to library: {}", lib_name));
    }
    return lib_ptr;
}

} // namespace cbagoa
