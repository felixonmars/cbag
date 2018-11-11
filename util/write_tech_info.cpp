#include <cbag/cbag.h>
#include <cbagoa/cbagoa.h>

void write_tech_info_file(const char *fname, const char *tech_lib, const char *lib_file = nullptr,
                          const char *pin_purpose = "pin") {
    std::string lib_str("cds.lib");
    if (lib_file != nullptr) {
        lib_str = lib_file;
    }

    cbagoa::oa_database db(lib_str);
    db.write_tech_info_file(fname, tech_lib, pin_purpose);
}

int main(int argc, char *argv[]) {
    switch (argc) {
    case 3:
        write_tech_info_file(argv[1], argv[2]);
        break;
    case 4:
        write_tech_info_file(argv[1], argv[2], argv[3]);
        break;
    case 5:
        write_tech_info_file(argv[1], argv[2], argv[3], argv[4]);
        break;
    default:
        std::cout << "Usage: write_tech_info <fname> <tech_lib> [<cds_lib_fname> [<pin_purpose>]]"
                  << std::endl;
    }

    return 0;
}
