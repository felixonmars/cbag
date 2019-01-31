#include <cbag/gdsii/read_util.h>
#include <cbag/util/io.h>

void print_gds(char *fname) {
    auto stream = cbag::util::open_file_read(fname, true);
    auto done = false;
    while (!done) {
        done = cbag::gdsii::print_record(stream);
    }
    std::cout << "print_gds done." << std::endl;
}

int main(int argc, char *argv[]) {
    switch (argc) {
    case 2:
        print_gds(argv[1]);
        break;
    default:
        std::cout << "Usage: print_gds <fname>" << std::endl;
    }

    return 0;
}
