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
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/functional/hash.hpp>

#include <oa/oaDesignDB.h>

// forward declare structures to reduce dependencies
namespace spdlog {
class logger;
} // namespace spdlog

namespace cbag {
namespace sch {
struct cellview;
struct instance;
} // namespace sch
} // namespace cbag

namespace cbagoa {

// constants
constexpr uint32_t LIB_ACCESS_TIMEOUT = 1;

// forward declare structures to reduce dependencies
class oa_reader;
class oa_writer;

using cell_key_t = std::pair<std::string, std::string>;
using cell_set_t = std::unordered_set<cell_key_t, boost::hash<cell_key_t>>;
using str_map_t = std::unordered_map<std::string, std::string>;

class LibDefObserver : public oa::oaObserver<oa::oaLibDefList> {
  public:
    explicit LibDefObserver(oa::oaUInt4 priority)
        : oa::oaObserver<oa::oaLibDefList>(priority, true){};

    oa::oaBoolean onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                 oa::oaLibDefListWarningTypeEnum type) override;
};

class oa_database {
  private:
    // OA namespace objects
    const oa::oaNativeNS ns{};
    const oa::oaCdbaNS ns_cdba{};

    const std::string lib_def_file;
    const LibDefObserver lib_def_obs{1};
    std::unique_ptr<oa_reader> reader;
    std::unique_ptr<oa_writer> writer;
    std::shared_ptr<spdlog::logger> logger;

  public:
    /** Open a OA database corresponding to the given library definition file.
     *
     *  @param lib_def_file the library definition file.
     */
    explicit oa_database(const char *lib_def_file);
    ~oa_database();
    oa_database(const oa_database &) = delete;
    oa_database &operator=(const oa_database &) = delete;
    oa_database(oa_database &&) = delete;
    oa_database &operator=(oa_database &&) = delete;

    explicit oa_database(const std::string &lib_def_file);

    std::vector<std::string> get_cells_in_library(const char *library);

    std::string get_lib_path(const char *library);

    /** Create a new library if it didn't already exist.
     *
     * @param library the library name.
     * @param lib_path directory to create the library at.
     * @param tech_lib the technology library name.
     */
    void create_lib(const char *library, const char *lib_path, const char *tech_lib);

    void create_lib(const std::string &library, const std::string &lib_path,
                    const std::string &tech_lib) {
        create_lib(library.c_str(), lib_path.c_str(), tech_lib.c_str());
    }

    cbag::sch::cellview read_sch_cellview(const char *lib_name, const char *cell_name,
                                          const char *view_name);

    cbag::sch::cellview read_sch_cellview(const std::string &lib_name, const std::string &cell_name,
                                          const std::string &view_name);

    std::vector<cell_key_t> read_sch_recursive(const char *lib_name, const char *cell_name,
                                               const char *view_name, const char *new_root_path,
                                               const str_map_t &lib_map,
                                               const std::unordered_set<std::string> &exclude_libs);

    std::vector<cell_key_t>
    read_sch_recursive(const std::string &lib_name, const std::string &cell_name,
                       const std::string &view_name, const std::string &new_root_path,
                       const str_map_t &lib_map,
                       const std::unordered_set<std::string> &exclude_libs) {
        return read_sch_recursive(lib_name.c_str(), cell_name.c_str(), view_name.c_str(),
                                  new_root_path.c_str(), lib_map, exclude_libs);
    }

    std::vector<cell_key_t> read_library(const char *lib_name, const char *view_name,
                                         const char *new_root_path, const str_map_t &lib_map,
                                         const std::unordered_set<std::string> &exclude_libs);

    void write_sch_cellview(const char *lib_name, const char *cell_name, const char *view_name,
                            bool is_sch, const cbag::sch::cellview &cv,
                            const str_map_t *rename_map = nullptr);

    void write_sch_cellview(const std::string &lib_name, const std::string &cell_name,
                            const std::string &view_name, bool is_sch,
                            const cbag::sch::cellview &cv, const str_map_t *rename_map = nullptr) {
        write_sch_cellview(lib_name.c_str(), cell_name.c_str(), view_name.c_str(), is_sch, cv,
                           rename_map);
    }

    void implement_sch_list(const char *lib_name, const std::vector<std::string> &cell_list,
                            const char *sch_view, const char *sym_view,
                            const std::vector<cbag::sch::cellview *> &cv_list);

  private:
    void handle_oa_exceptions();

    oa::oaTech *read_tech(const char *library);

    oa::oaDesign *open_design(const char *lib_name, const char *cell_name, const char *view_name,
                              char mode = 'r', bool is_sch = true);

    void read_sch_helper(std::pair<std::string, std::string> &key, const char *view_name,
                         const char *new_root_path, const str_map_t &lib_map,
                         const std::unordered_set<std::string> &exclude_libs,
                         cell_set_t &exclude_cells, std::vector<cell_key_t> &cell_list);

    cbag::sch::cellview cell_to_yaml(const std::string &lib_name, const std::string &cell_name,
                                     const char *sch_view, const std::string &root_path);
};

} // namespace cbagoa

#endif // CBAGOA_DATABASE_H
