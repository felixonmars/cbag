#ifndef CBAG_COMMON_PARAM_MAP_H
#define CBAG_COMMON_PARAM_MAP_H

#include <cbag/common/param_data.h>
#include <cbag/util/sorted_map.h>

namespace cbag {

/** Type definition for a parameter dictonary.
 */
using param_map = util::sorted_map<std::string, value_t>;

} // namespace cbag

#endif
