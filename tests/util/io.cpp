#include <fstream>
#include <sstream>

#include "util/io.h"

std::string read_file(const std::string &fname) {
    std::ifstream in(fname);
    std::stringstream stream;
    stream << in.rdbuf();
    return stream.str();
}
