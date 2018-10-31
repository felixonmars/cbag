
#include <boost/filesystem.hpp>

#include <cbagyaml/cbagyaml.h>

#include <cbagoa/oa_read.h>
#include <cbagoa/oa_read_lib.h>

namespace fs = boost::filesystem;

namespace cbagoa {

cbag::sch::cellview cell_to_yaml(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                                 spdlog::logger &logger, const std::string &lib_name,
                                 const std::string &cell_name, const std::string &sch_view,
                                 const std::string &root_path) {
    // create directory if not exist, then compute output filename
    fs::path root_dir(fs::path(root_path) / lib_name / "netlist_info");
    fs::create_directories(root_dir);

    // parse schematic
    cbag::sch::cellview sch_cv =
        read_sch_cellview(ns_native, ns, logger, lib_name, cell_name, sch_view);

    // write schematic to file
    fs::path tmp_path = root_dir / fmt::format("{}.yaml", cell_name);
    cbag::to_file(sch_cv, tmp_path.c_str());

    // write all symbol views to file
    // get library read access
    oa::oaLib *lib_ptr = open_library_read(ns_native, lib_name);
    // find all symbol views
    oa::oaScalarName cell_name_oa(ns_native, cell_name.c_str());
    oa::oaCell *cell_ptr = oa::oaCell::find(lib_ptr, cell_name_oa);
    oa::oaIter<oa::oaCellView> cv_iter(cell_ptr->getCellViews());
    oa::oaCellView *cv_ptr;
    while ((cv_ptr = cv_iter.getNext()) != nullptr) {
        oa::oaString tmp_name;
        oa::oaView *view_ptr = cv_ptr->getView();
        if (view_ptr->getViewType() == oa::oaViewType::get(oa::oacSchematicSymbol)) {
            view_ptr->getName(ns_native, tmp_name);
            tmp_path = root_dir / fmt::format("{}.{}.yaml", cell_name, (const char *)tmp_name);
            cbag::to_file(read_sch_cellview(ns_native, ns, logger, lib_name, cell_name,
                                            std::string((const char *)tmp_name)),
                          tmp_path.c_str());
        }
    }
    // release read access
    lib_ptr->releaseAccess();

    return sch_cv;
}

} // namespace cbagoa
