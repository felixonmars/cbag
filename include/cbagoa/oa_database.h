/** \file database.h
 *  \brief This file defines classes and functions to read/write an OpenAccess
 * database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAGOA_OA_DATABASE_H
#define CBAGOA_OA_DATABASE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

// forward declare structures to reduce dependencies
class oa_reader;
class oa_writer;

using cell_key_t = std::pair<std::string, std::string>;
using str_map_t = std::unordered_map<std::string, std::string>;
using sch_cv_info = std::pair<std::string, cbag::sch::cellview *>;
using lay_cv_info = std::pair<std::string, cbag::layout::cellview *>;

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
    std::string lib_def_file;
    LibDefObserver lib_def_obs{1};

  public:
    oa::oaNativeNS ns_native;
    oa::oaCdbaNS ns;
    str_map_t yaml_path_map;
    std::unordered_set<std::string> primitive_libs;
    std::shared_ptr<spdlog::logger> logger;

  public:
    /** Open a OA database corresponding to the given library definition file.
     *
     *  @param lib_def_file the library definition file.
     */
    explicit oa_database(std::string lib_def_fname);
    ~oa_database();

    void add_yaml_path(const std::string &lib_name, std::string yaml_path);

    void add_primitive_lib(std::string lib_name);

    bool is_primitive_lib(const std::string &lib_name) const;

    std::vector<std::string> get_cells_in_lib(const std::string &lib_name) const;

    std::string get_lib_path(const std::string &lib_name) const;

    void create_lib(const std::string &lib_name, const std::string &lib_path,
                    const std::string &tech_lib) const;

    cbag::sch::cellview read_sch_cellview(const std::string &lib_name, const std::string &cell_name,
                                          const std::string &view_name) const;

    std::vector<cell_key_t> read_sch_recursive(const std::string &lib_name,
                                               const std::string &cell_name,
                                               const std::string &view_name) const;

    std::vector<cell_key_t> read_library(const std::string &lib_name,
                                         const std::string &view_name) const;

    void write_sch_cellview(const std::string &lib_name, const std::string &cell_name,
                            const std::string &view_name, bool is_sch,
                            const cbag::sch::cellview &cv,
                            const str_map_t *rename_map = nullptr) const;

    void write_lay_cellview(const std::string &lib_name, const std::string &cell_name,
                            const std::string &view_name, const cbag::layout::cellview &cv,
                            oa::oaTech *tech, const str_map_t *rename_map = nullptr) const;

    void implement_sch_list(const std::string &lib_name, const std::string &sch_view,
                            const std::string &sym_view,
                            const std::vector<sch_cv_info> &cv_list) const;

    void implement_lay_list(const std::string &lib_name, const std::string &view,
                            const std::vector<lay_cv_info> &cv_list) const;

    void write_tech_info_file(const std::string &fname, const std::string &tech_lib,
                              const std::string &pin_purpose) const;
};

} // namespace cbagoa

#endif // CBAGOA_DATABASE_H
