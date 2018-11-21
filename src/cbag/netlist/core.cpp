/** \file netlist.h
 *  \brief This file defines classes that netlists objects.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#include <boost/filesystem.hpp>

#include <cbag/netlist/core.h>
#include <cbag/util/io.h>

namespace fs = boost::filesystem;

namespace cbag {
namespace netlist {

nstream_file::nstream_file(const std::string &fname) : out_file(util::open_file_write(fname)) {}

void nstream_file::close() { out_file.close(); }

} // namespace netlist
} // namespace cbag
