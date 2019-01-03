#ifndef CBAG_YAML_VECTOR_H
#define CBAG_YAML_VECTOR_H

#include <cbag/common/vector.h>
#include <cbag/yaml/int_array.h>

namespace YAML {

template <> struct convert<cbag::vector> : public convert_arr<cbag::offset_t, 2> {};

} // namespace YAML

#endif
