//
// Created by erichang on 6/26/18.
//

#ifndef CBAGOA_DATABASE_H
#define CBAGOA_DATABASE_H

#include <map>
#include <string>

#include <oa/oaDesignDB.h>
#include <cbag/schematic/objects.h>

// techID = techOpenTechFile(lib_name "tech.oa" "r")
// techGetPurposeNum(techID "pin")

namespace cbagoa {

    class LibDefObserver : public oa::oaObserver<oa::oaLibDefList> {
    public:
        explicit LibDefObserver(oa::oaUInt4 priority)
                :
                oa::oaObserver<oa::oaLibDefList>(priority, true) {};

        oa::oaBoolean onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                     oa::oaLibDefListWarningTypeEnum type) override;
    };

    class Library {
    public:
        Library()
                :
                formatter(cbag::NameFormatter(',', '<', '>', ':')), is_open(false), dbu_per_uu(1000),
                lib_def_obs(1), lib_ptr(nullptr), tech_ptr(nullptr) {}

        ~Library();

        void open_lib(const std::string &lib_file, const std::string &library,
                      const std::string &lib_path, const std::string &tech_lib);

        cbag::CSchMaster parse_sch(const std::string &cell_name,
                                   const std::string &view_name = "schematic");

        void close();

    private:

        cbag::Name make_name(oa::oaTerm *term_ptr);

        // OA namespace objects
        const oa::oaNativeNS ns;
        const oa::oaCdbaNS ns_cdba;
        cbag::NameFormatter formatter;

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
