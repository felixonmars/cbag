
#include <cbag/netlist/core.h>
#include <cbag/util/io.h>

namespace cbag {
namespace netlist {

nstream_file::nstream_file(const std::string &fname) : out_file(util::open_file_write(fname)) {}

void nstream_file::close() {
    out_file.flush();
    out_file.close();
}

} // namespace netlist
} // namespace cbag
