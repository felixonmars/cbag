
#ifndef CBAG_COMMON_TEXT_DISP_FORMAT_H
#define CBAG_COMMON_TEXT_DISP_FORMAT_H

#include <cstdint>

namespace cbag {

enum class text_disp_format : uint8_t { name, value, nameValue };
constexpr text_disp_format tdfName = text_disp_format::name;
constexpr text_disp_format tdfValue = text_disp_format::value;
constexpr text_disp_format tdfNameValue = text_disp_format::nameValue;

} // namespace cbag

#endif
