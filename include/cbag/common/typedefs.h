
#ifndef CBAG_COMMON_TYPEDEFS_H
#define CBAG_COMMON_TYPEDEFS_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using coord_t = oa::oaCoord;
using lay_t = oa::oaLayerNum;
using purp_t = oa::oaPurposeNum;
using dist_t = oa::oaDist;
using offset_t = oa::oaOffset;

} // namespace cbag

#else

namespace cbag {

using coord_t = int32_t;
using lay_t = uint32_t;
using purp_t = uint32_t;
using dist_t = uint32_t;
using offset_t = int32_t;

} // namespace cbag

#endif
#endif
