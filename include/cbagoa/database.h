/** \file database.h
 *  \brief This file defines classes and functions to read/write an OpenAccess
 * database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAGOA_DATABASE_H
#define CBAGOA_DATABASE_H

#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include <boost/functional/hash.hpp>

#include <oa/oaDesignDB.h>

namespace cbag {
// forward declare structures to reduce dependencies
struct SchCellView;
struct Instance;
} // namespace cbag

namespace cbagoa {

// forward declare structures to reduce dependencies
class OAReader;
class OAWriter;

using cell_key_t = std::pair<std::string, std::string>;
using cell_set_t = std::unordered_set<cell_key_t, boost::hash<cell_key_t>>;

class LibDefObserver : public oa::oaObserver<oa::oaLibDefList> {
  public:
    explicit LibDefObserver(oa::oaUInt4 priority)
        : oa::oaObserver<oa::oaLibDefList>(priority, true){};

    oa::oaBoolean onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                 oa::oaLibDefListWarningTypeEnum type) override;
};

class OADatabase {
  public:
    /** Open a OA database corresponding to the given library definition file.
     *
     *  @param lib_def_file the library definition file.
     */
    explicit OADatabase(const char *lib_def_file);

    explicit OADatabase(const std::string &lib_def_file)
        : OADatabase(lib_def_file.c_str()) {}

    ~OADatabase();

    // This class is neither copyable nor movable

    OADatabase(const OADatabase &) = delete;

    OADatabase &operator=(const OADatabase &) = delete;

    OADatabase(OADatabase &&) = delete;

    OADatabase &operator=(OADatabase &&) = delete;

    std::vector<std::string> get_cells_in_library(const char *library);

    std::string get_lib_path(const char *library);

    /** Create a new library if it didn't already exist.
     *
     * @param library the library name.
     * @param lib_path directory to create the library at.
     * @param tech_lib the technology library name.
     */
    void create_lib(const char *library, const char *lib_path,
                    const char *tech_lib);

    void create_lib(const std::string &library, const std::string &lib_path,
                    const std::string &tech_lib) {
        create_lib(library.c_str(), lib_path.c_str(), tech_lib.c_str());
    }

    cbag::SchCellView read_sch_cellview(const char *lib_name,
                                        const char *cell_name,
                                        const char *view_name);

    cbag::SchCellView read_sch_cellview(const std::string &lib_name,
                                        const std::string &cell_name,
                                        const std::string &view_name);

    std::vector<cell_key_t> read_sch_recursive(
        const char *lib_name, const char *cell_name, const char *view_name,
        const char *new_root_path,
        const std::unordered_map<std::string, std::string> &lib_map,
        const std::unordered_set<std::string> &exclude_libs);

    std::vector<cell_key_t> read_sch_recursive(
        const std::string &lib_name, const std::string &cell_name,
        const std::string &view_name, const std::string &new_root_path,
        const std::unordered_map<std::string, std::string> &lib_map,
        const std::unordered_set<std::string> &exclude_libs) {
        return read_sch_recursive(lib_name.c_str(), cell_name.c_str(),
                                  view_name.c_str(), new_root_path.c_str(),
                                  lib_map, exclude_libs);
    }

    void write_sch_cellview(const char *lib_name, const char *cell_name,
                            const char *view_name, bool is_sch,
                            const cbag::SchCellView &cv);

    void write_sch_cellview(const std::string &lib_name,
                            const std::string &cell_name,
                            const std::string &view_name, bool is_sch,
                            const cbag::SchCellView &cv) {
        write_sch_cellview(lib_name.c_str(), cell_name.c_str(),
                           view_name.c_str(), is_sch, cv);
    }

  private:
    oa::oaTech *read_tech(const char *library);

    oa::oaDesign *open_design(const char *lib_name, const char *cell_name,
                              const char *view_name, char mode = 'r',
                              bool is_sch = true);

    void
    read_sch_helper(std::pair<std::string, std::string> &key,
                    const char *view_name, const char *new_root_path,
                    const std::unordered_map<std::string, std::string> &lib_map,
                    const std::unordered_set<std::string> &exclude_libs,
                    cell_set_t &exclude_cells,
                    std::vector<cell_key_t> &cell_list);

    void read_prim_instance(const char *fname, const cbag::Instance &inst);

    // OA namespace objects
    const oa::oaNativeNS ns;
    const oa::oaCdbaNS ns_cdba;

    const std::string lib_def_file;
    const LibDefObserver lib_def_obs;
    std::unique_ptr<OAReader> reader;
    std::unique_ptr<OAWriter> writer;
};
} // namespace cbagoa

#endif // CBAGOA_DATABASE_H
