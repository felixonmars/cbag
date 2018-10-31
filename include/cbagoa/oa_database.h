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

namespace layout {
struct cellview;
}
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
    const oa::oaNativeNS ns;
    const oa::oaCdbaNS ns_cdba;

    const std::string lib_def_file;
    const LibDefObserver lib_def_obs{1};
    std::unique_ptr<oa_writer> writer;
    std::shared_ptr<spdlog::logger> logger;
    struct helper;

  public:
    /** Open a OA database corresponding to the given library definition file.
     *
     *  @param lib_def_file the library definition file.
     */
    explicit oa_database(const char *lib_def_file);
    ~oa_database();

    explicit oa_database(const std::string &lib_def_file);

    std::vector<std::string> get_cells_in_library(const char *library) const;

    std::string get_lib_path(const char *library) const;

    void create_lib(const char *library, const char *lib_path, const char *tech_lib) const;

    cbag::sch::cellview read_sch_cellview(const char *lib_name, const char *cell_name,
                                          const char *view_name) const;

    std::vector<cell_key_t>
    read_sch_recursive(const char *lib_name, const char *cell_name, const char *view_name,
                       const char *new_root_path, const str_map_t &lib_map,
                       const std::unordered_set<std::string> &exclude_libs) const;

    std::vector<cell_key_t> read_library(const char *lib_name, const char *view_name,
                                         const char *new_root_path, const str_map_t &lib_map,
                                         const std::unordered_set<std::string> &exclude_libs) const;

    void write_sch_cellview(const char *lib_name, const char *cell_name, const char *view_name,
                            bool is_sch, const cbag::sch::cellview &cv,
                            const str_map_t *rename_map = nullptr) const;

    void implement_sch_list(const char *lib_name, const std::vector<std::string> &cell_list,
                            const char *sch_view, const char *sym_view,
                            const std::vector<cbag::sch::cellview *> &cv_list) const;

    void write_lay_cellview(const char *lib_name, const char *cell_name, const char *view_name,
                            const cbag::layout::cellview &cv, const str_map_t *rename_map = nullptr,
                            oa::oaTech *tech_ptr = nullptr) const;

    void implement_lay_list(const char *lib_name, const std::vector<std::string> &cell_list,
                            const char *view,
                            const std::vector<cbag::layout::cellview *> &cv_list) const;

    void write_tech_info_file(const char *fname, const char *tech_lib, const char *pin_purpose);
};

} // namespace cbagoa

#endif // CBAGOA_DATABASE_H
