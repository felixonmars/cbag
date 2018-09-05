#include <fstream>

#include <cbag/cbag.h>
#include <cbagoa/cbagoa.h>

void read_oa() {
    cbag::init_logging();

    std::string lib_file("cds.lib");
    std::string lib_path(".");
    std::string tech_lib("tsmcN16");
    std::string lib_name("AAAFOO_TEST_STRONGARM");
    std::string cell_name("inv");
    std::string cell_name2("inv2");
    std::string view_name("schematic");
    std::string sym_view_name("symbol");

    cbagoa::oa_database db(lib_file);

    cbag::sch::cellview sch_master = db.read_sch_cellview(lib_name, cell_name, view_name);
    db.write_sch_cellview(lib_name, cell_name2, view_name, true, sch_master);
    cbag::sch::cellview sym_master = db.read_sch_cellview(lib_name, cell_name, sym_view_name);
    db.write_sch_cellview(lib_name, cell_name2, sym_view_name, false, sym_master);

    db.read_sch_cellview(lib_name, cell_name2, view_name);
    db.read_sch_cellview(lib_name, cell_name2, sym_view_name);
}


void test_oa_box() {

    oa::oaBox box(0, 0, 5, 7);
    oa::oaPoint p;
    box.getCenter(p);
    std::cout << "center x: " << p.x() << std::endl;
    std::cout << "center y: " << p.y() << std::endl;
}


int main(int argc, char *argv[]) {
    // read_oa();

    test_oa_box();

    return 0;
}
