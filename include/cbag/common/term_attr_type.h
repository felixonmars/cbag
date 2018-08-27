
#ifndef CBAG_COMMON_TERM_ATTR_TYPE_H
#define CBAG_COMMON_TERM_ATTR_TYPE_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using term_attr_type = oa::oaTermAttrTypeEnum;
constexpr term_attr_type tatName = term_attr_type::oacNameTermAttrType;
constexpr term_attr_type tatHasPin = term_attr_type::oacHasPinsTermAttrType;
constexpr term_attr_type tatNumBits = term_attr_type::oacNumBitsTermAttrType;

} // namespace cbag

#else

namespace cbag {

enum term_attr_type { name, hasPins, numBits };
constexpr term_attr_type tatName = term_attr_type::name;
constexpr term_attr_type tatHasPin = term_attr_type::hasPins;
constexpr term_attr_type tatNumBits = term_attr_type::numBits;

} // namespace cbag

#endif
#endif
