/** \file database.h
 *  \brief This file defines classes and functions to read/write an OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAGOA_DATABASE_H
#define CBAGOA_DATABASE_H

#include <map>
#include <string>

#include <oa/oaDesignDB.h>

#include <cbag/database/cellviews.h>


namespace cbagoa {

    class LibDefObserver : public oa::oaObserver<oa::oaLibDefList> {
    public:
        explicit LibDefObserver(oa::oaUInt4 priority)
                :
                oa::oaObserver<oa::oaLibDefList>(priority, true) {};

        oa::oaBoolean onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                     oa::oaLibDefListWarningTypeEnum type) override;
    };

    class OALibrary {
    public:
        OALibrary()
                : is_open(false), dbu_per_uu(1000), lib_def_obs(1),
                  lib_ptr(nullptr), tech_ptr(nullptr) {}

        ~OALibrary();

        // This class is neither copyable nor movable

        OALibrary(const OALibrary &) = delete;

        OALibrary &operator=(const OALibrary &) = delete;

        OALibrary(OALibrary &&) = delete;

        OALibrary &operator=(OALibrary &&) = delete;

        void open_lib(const std::string &lib_file, const std::string &library,
                      const std::string &lib_path, const std::string &tech_lib);

        cbag::SchMaster parse_schematic(const std::string &cell_name,
                                         const std::string &view_name = "schematic");

        void parse_symbol(const std::string &cell_name,
                          const std::string &view_name = "symbol");

        void close();

    private:

        std::pair<oa::oaDesign *, oa::oaBlock *>
        open_design(const std::string &cell_name, const std::string &view_name,
                    oa::oaReservedViewTypeEnum view_type);

        // OA namespace objects
        const oa::oaNativeNS ns;
        const oa::oaCdbaNS ns_cdba;

        bool is_open;
        oa::oaUInt4 dbu_per_uu;
        LibDefObserver lib_def_obs;

        oa::oaLib *lib_ptr;
        oa::oaTech *tech_ptr;
        std::string lib_name;
        oa::oaScalarName lib_name_oa;
    };

    void add_param(cbag::ParamMap &params, oa::oaProp *prop_ptr);

}

#endif //CBAGOA_DATABASE_H
