#ifndef CBAG_LAYOUT_TECH_UTIL_H
#define CBAG_LAYOUT_TECH_UTIL_H

#include <string>

#include <cbag/common/layer_t.h>
#include <cbag/layout/tech.h>

namespace cbag {
namespace layout {

layer_t get_layer_t(const tech &t, const std::string &layer, const std::string &purpose);

} // namespace layout
} // namespace cbag

#endif
