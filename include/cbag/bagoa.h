//
// Created by erichang on 6/26/18.
//

#ifndef CBAG_BAGOA_H
#define CBAG_BAGOA_H

#include <map>
#include <string>

#include <oa/oaDesignDB.h>

// techID = techOpenTechFile(lib_name "tech.oa" "r")
// techGetPurposeNum(techID "pin")

namespace cbag {
    class LibDefObserver: public oa::oaObserver<oa::oaLibDefList> {
    public:
        explicit LibDefObserver(oa::oaUInt4 priority) :
                oa::oaObserver<oa::oaLibDefList>(priority, true) {
        }
        ;

        oa::oaBoolean onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString & msg,
                                     oa::oaLibDefListWarningTypeEnum type);
    };

    class OALibrary {
    public:
        OALibrary() :
                is_open(false), dbu_per_uu(1000), lib_def_obs(1), lib_ptr(nullptr), tech_ptr(nullptr) {
        }
        ~OALibrary() = default;

        void open_library(const std::string & lib_file, const std::string & library,
                          const std::string & lib_path, const std::string & tech_lib);

        void close();

        bool is_open;
        oa::oaUInt4 dbu_per_uu;
        LibDefObserver lib_def_obs;

        oa::oaLib * lib_ptr;
        oa::oaTech * tech_ptr;
        oa::oaScalarName lib_name;
    };

}


#endif //CBAG_BAGOA_H
