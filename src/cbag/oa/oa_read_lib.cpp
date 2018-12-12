
#include <cbag/schematic/cellview.h>

#include <cbag/oa/oa_read.h>
#include <cbag/oa/oa_read_lib.h>
#include <cbag/util/io.h>

namespace cbagoa {

cbag::sch::cellview cell_to_yaml(const oa::oaNativeNS &ns_native, const oa::oaCdbaNS &ns,
                                 spdlog::logger &logger, const std::string &lib_name,
                                 const std::string &cell_name, const std::string &sch_view,
                                 const std::string &yaml_path,
                                 const std::unordered_set<std::string> &primitive_libs) {
    // parse schematic
    cbag::sch::cellview sch_cv =
        read_sch_cellview(ns_native, ns, logger, lib_name, cell_name, sch_view, primitive_libs);

    // write schematic to file
    auto yaml_dir = cbag::util::get_canonical_path(yaml_path);
    auto yaml_fpath = yaml_dir / (cell_name + ".yaml");
    cbag::util::make_parent_dirs(yaml_fpath);
    sch_cv.to_file(yaml_fpath.string());

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
            yaml_fpath = yaml_dir / fmt::format("{}.{}.yaml", cell_name, (const char *)tmp_name);
            read_sch_cellview(ns_native, ns, logger, lib_name, cell_name,
                              std::string((const char *)tmp_name), primitive_libs)
                .to_file(yaml_fpath.string());
        }
    }
    // release read access
    lib_ptr->releaseAccess();

    return sch_cv;
}

} // namespace cbagoa
