#ifndef CBAGOA_OA_READ_LIB_H
#define CBAGOA_OA_READ_LIB_H

#include <fmt/format.h>

#include <oa/oaDesignDB.h>

#include <cbagoa/oa_util.h>

namespace cbagoa {

template <class OutIter>
void get_cells(const oa::oaCdbaNS &ns, spdlog::logger &logger, const std::string &lib_name,
               OutIter out_iter) {
    try {
        oa::oaLib *lib_ptr = open_library_read(ns, lib_name);
        oa::oaIter<oa::oaCell> cell_iter(lib_ptr->getCells());
        oa::oaCell *cell_ptr;
        oa::oaString tmp_str;
        while ((cell_ptr = cell_iter.getNext()) != nullptr) {
            cell_ptr->getName(ns, tmp_str);
            out_iter = std::string(tmp_str);
        }
        lib_ptr->releaseAccess();
    } catch (...) {
        handle_oa_exceptions(logger);
    }
}



} // namespace cbagoa

#endif
