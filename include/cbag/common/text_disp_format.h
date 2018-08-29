
#ifndef CBAG_COMMON_TEXT_DISP_FORMAT_H
#define CBAG_COMMON_TEXT_DISP_FORMAT_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using text_disp_format = oa::oaTextDisplayFormatEnum;
constexpr text_disp_format tdfName = text_disp_format::oacNameTextDisplayFormat;
constexpr text_disp_format tdfValue = text_disp_format::oacValueTextDisplayFormat;
constexpr text_disp_format tdfNameValue = text_disp_format::oacNameValueTextDisplayFormat;

} // namespace cbag

#else

namespace cbag {

enum text_disp_format : uint32_t { name, value, nameValue };
constexpr text_disp_format tdfName = text_disp_format::name;
constexpr text_disp_format tdfValue = text_disp_format::value;
constexpr text_disp_format tdfNameValue = text_disp_format::nameValue;

} // namespace cbag

#endif
#endif
