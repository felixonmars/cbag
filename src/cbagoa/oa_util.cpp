
#include <cbag/logging/spdlog.h>

#include <oa/oaDesignDB.h>

#include <cbagoa/oa_util.h>

namespace cbagoa {

oa::oaTech *read_tech(const oa::oaNativeNS &ns, const std::string &lib_name) {
    // open technology file
    oa::oaScalarName lib_name_oa = oa::oaScalarName(ns, lib_name.c_str());
    oa::oaTech *tech_ptr = oa::oaTech::find(lib_name_oa);
    if (tech_ptr == nullptr) {
        // opened tech not found, attempt to open
        if (!oa::oaTech::exists(lib_name_oa)) {
            throw std::runtime_error(fmt::format("Cannot find technology library: {}", lib_name));
        } else {
            tech_ptr = oa::oaTech::open(lib_name_oa, 'r');
            if (tech_ptr == nullptr) {
                throw std::runtime_error(
                    fmt::format("Cannot open technology library: {}", lib_name));
            }
        }
    }
    return tech_ptr;
}

oa::oaDesign *open_design(const oa::oaNativeNS &ns, spdlog::logger &logger,
                          const std::string &lib_name, const std::string &cell_name,
                          const std::string &view_name, char mode,
                          oa::oaReservedViewTypeEnum view_enum) {
    oa::oaScalarName lib_oa(ns, lib_name.c_str());
    oa::oaScalarName cell_oa(ns, cell_name.c_str());
    oa::oaScalarName view_oa(ns, view_name.c_str());

    logger.info("Opening design {}__{}({}) with mode {}", lib_name, cell_name, view_name, mode);
    oa::oaDesign *dsn_ptr = nullptr;
    if (mode == 'r') {
        dsn_ptr = oa::oaDesign::open(lib_oa, cell_oa, view_oa, mode);
    } else {
        oa::oaViewType *view_type = oa::oaViewType::get(view_enum);
        dsn_ptr = oa::oaDesign::open(lib_oa, cell_oa, view_oa, view_type, mode);
    }
    if (dsn_ptr == nullptr) {
        throw std::invalid_argument(fmt::format("Cannot open cell: {}__{}({}) with mode {}",
                                                lib_name, cell_name, view_name, mode));
    }
    return dsn_ptr;
}

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
oa::oaLib *open_library_read(const oa::oaNativeNS &ns, const std::string &lib_name) {
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
