#include <cbag/cbag.h>
#include <cbagoa/cbagoa.h>

void write_cv_yaml(const char *lib_name, const char *cell_name, const char *view_name = "symbol",
                   const char *output = nullptr, const char *lib_file = "cds.lib") {
    std::string out_fname;
    if (output == nullptr)
        out_fname = fmt::format("{}.yaml", cell_name);
    else
        out_fname = output;

    cbagoa::oa_database db(lib_file);
    auto cv = db.read_sch_cellview(lib_name, cell_name, view_name);

    cv.to_file(out_fname);
}

int main(int argc, char *argv[]) {
    switch (argc) {
    case 3:
        write_cv_yaml(argv[1], argv[2]);
        break;
    case 4:
        write_cv_yaml(argv[1], argv[2], argv[3]);
        break;
    case 5:
        write_cv_yaml(argv[1], argv[2], argv[3], argv[4]);
        break;
    case 6:
        write_cv_yaml(argv[1], argv[2], argv[3], argv[4], argv[5]);
        break;
    default:
        std::cout << "Usage: write_cv_yaml <lib_name> <cell_name> [<view_name> [<output> "
                     "[<cds_lib_fname>]]]"
                  << std::endl;
    }

    return 0;
}
