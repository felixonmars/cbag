
#ifndef CBAG_COMMON_TYPEDEFS_H
#define CBAG_COMMON_TYPEDEFS_H

#include <utility>

#if __has_include(<oa/oaDesignDB.h>)
#include <oa/oaDesignDB.h>
#endif

namespace cbag {

#if __has_include(<oa/oaDesignDB.h>)
using coord_t = oa::oaCoord;
using lay_t = oa::oaLayerNum;
using purp_t = oa::oaPurposeNum;
using dist_t = oa::oaDist;
using offset_t = oa::oaOffset;
#else
using coord_t = int32_t;
using lay_t = uint32_t;
using purp_t = uint32_t;
using dist_t = uint32_t;
using offset_t = int32_t;
#endif

using layer_t = std::pair<lay_t, purp_t>;

// custom unsigned char literal definition
inline constexpr unsigned char operator"" _uc(unsigned long long arg) noexcept {
    return static_cast<unsigned char>(arg);
}
} // namespace cbag

#endif
