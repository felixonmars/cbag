/** \file database.cpp
 *  \brief This file implements classes and functions to read/write an OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

// TODO: Debug only, remve later
#include <iostream>

#include <fmt/format.h>
// include fmt/ostream.h to support formatting oaStrings, which defines operator <<
#include <fmt/ostream.h>

#include <cbagoa/database.h>
#include <cbagoa/convert.h>


namespace cbagoa {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

    oa::oaBoolean LibDefObserver::onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                                 oa::oaLibDefListWarningTypeEnum type) {
        throw std::runtime_error("OA Error: " + std::string(msg));
    }

#pragma clang diagnostic pop

    OALibrary::~OALibrary() {
        close();
    }

    void OALibrary::open_lib(const std::string &lib_file, const std::string &library,
                             const std::string &lib_path, const std::string &tech_lib) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "NotAssignable"
        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, // NOLINT
                     oacDataModelRevNumber); // NOLINT
#pragma clang diagnostic pop

        // open library definition
        oa::oaString lib_def_file(lib_file.c_str());
        oa::oaLibDefList::openLibs(lib_def_file);

        // open library
        lib_name = library;
        lib_name_oa = oa::oaScalarName(ns, library.c_str());
        lib_ptr = oa::oaLib::find(lib_name_oa);
        if (lib_ptr == nullptr) {
            // create new library
            oa::oaString oa_lib_path(lib_path.c_str());
            oa::oaScalarName oa_tech_lib(ns, tech_lib.c_str());
            lib_ptr = oa::oaLib::create(lib_name_oa, oa_lib_path);
            oa::oaTech::attach(lib_ptr, oa_tech_lib);

            // NOTE: I cannot get open access to modify the library file, so
            // we just do it by hand.
            std::ofstream outfile;
            outfile.open(lib_file, std::ios_base::app);
            outfile << "DEFINE " << library << " " << lib_path << std::endl;
            outfile.close();
        } else if (!lib_ptr->isValid()) {
            throw std::invalid_argument("Invalid library: " + library);
        }

        // open technology file
        tech_ptr = oa::oaTech::find(lib_ptr);
        if (tech_ptr == nullptr) {
            // opened tech not found, attempt to open
            if (!oa::oaTech::exists(lib_ptr)) {
                throw std::runtime_error("Cannot find technology for library: " + library);
            } else {
                tech_ptr = oa::oaTech::open(lib_ptr, 'r');
                if (tech_ptr == nullptr) {
                    throw std::runtime_error("Cannot open technology for library: " + library);
                }
            }
        }

        // get database unit
        dbu_per_uu = tech_ptr->getDBUPerUU(oa::oaViewType::get(oa::oacMaskLayout));

        is_open = true;
    }

    oa::oaDesign *
    OALibrary::open_design(const std::string &cell_name, const std::string &view_name) {
        oa::oaScalarName cell_oa(ns, cell_name.c_str());
        oa::oaScalarName view_oa(ns, view_name.c_str());

        oa::oaDesign *dsn_ptr = oa::oaDesign::open(lib_name_oa, cell_oa, view_oa, 'r');
        if (dsn_ptr == nullptr) {
            throw std::invalid_argument(fmt::format("Cannot open cell: {}__{}({})",
                                                    lib_name, cell_name, view_name));
        }
        return dsn_ptr;
    }

    cbag::SchCellView OALibrary::parse_sch_cell_view(const std::string &cell_name,
                                                     const std::string &view_name) {
        // get OA design and block pointers
        oa::oaDesign *design = open_design(cell_name, view_name);

        return read_sch_cell_view(design, ns_cdba);
    }

    void OALibrary::close() {
        if (is_open) {
            tech_ptr->close();
            lib_ptr->close();

            is_open = false;
        }

    }
}