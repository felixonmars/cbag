#include <fstream>

#include <cbagoa/cbagoa.h>

int read_oa() {
    cbag::init_logging();

    std::string lib_file("cds.lib");
    std::string lib_path(".");
    std::string tech_lib("tsmcN16");
    std::string lib_name("AAAFOO_TEST_STRONGARM");
    std::string cell_name("inv");
    std::string cell_name2("inv2");
    std::string view_name("schematic");
    std::string sym_view_name("symbol");

    cbagoa::OADatabase db(lib_file);

    cbag::SchCellView sch_master = db.read_sch_cellview(lib_name, cell_name, view_name);
    db.write_sch_cellview(lib_name, cell_name2, view_name, true, sch_master);
    cbag::SchCellView sym_master = db.read_sch_cellview(lib_name, cell_name, sym_view_name);
    db.write_sch_cellview(lib_name, cell_name2, sym_view_name, false, sym_master);

    return 0;
}

int main(int argc, char *argv[]) { return read_oa(); }
