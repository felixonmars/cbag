#ifndef CBAG_OA_TYPEDEF_H
#define CBAG_OA_TYPEDEF_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <boost/functional/hash.hpp>

namespace cbag {
namespace layout {
class cellview;
}
namespace sch {
struct cellview;
}
} // namespace cbag

namespace cbagoa {

using cell_key_t = std::pair<std::string, std::string>;
using str_map_t = std::unordered_map<std::string, std::string>;
using cell_set_t = std::unordered_set<cell_key_t, boost::hash<cell_key_t>>;

using sch_cv_info = std::pair<std::string, std::pair<cbag::sch::cellview *, std::string>>;
using lay_cv_info = std::pair<std::string, cbag::layout::cellview *>;

} // namespace cbagoa

#endif
