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

#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

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

    class OADatabase {
    public:
        /** Open a OA database corresponding to the given library definition file.
         *
         *  @param lib_def_file the library definition file.
         */
        explicit OADatabase(const std::string &lib_def_file);

        // This class is neither copyable nor movable

        OADatabase(const OADatabase &) = delete;

        OADatabase &operator=(const OADatabase &) = delete;

        OADatabase(OADatabase &&) = delete;

        OADatabase &operator=(OADatabase &&) = delete;

        /** Create a new library if it didn't already exist.
         *
         * @param library the library name.
         * @param lib_path directory to create the library at.
         * @param tech_lib the technology library name.
         */
        void create_lib(const std::string &library, const std::string &lib_path,
                        const std::string &tech_lib);

        cbag::SchCellView read_sch_cellview(const std::string &lib_name,
                                            const std::string &cell_name,
                                            const std::string &view_name);

    private:

        oa::oaTech *read_tech(const std::string &library);

        oa::oaDesign *read_design(const std::string &lib_name, const std::string &cell_name,
                                  const std::string &view_name);

        // OA namespace objects
        const oa::oaNativeNS ns;
        const oa::oaCdbaNS ns_cdba;

        const std::string lib_def_file;
        const LibDefObserver lib_def_obs;
        const std::unique_ptr<g3::LogWorker> log_worker;
    };
}

#endif //CBAGOA_DATABASE_H
