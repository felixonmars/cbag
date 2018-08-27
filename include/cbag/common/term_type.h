
#ifndef CBAG_COMMON_TERM_TYPE_H
#define CBAG_COMMON_TERM_TYPE_H

#if __has_include(<oa/oaDesignDB.h>)

#include <oa/oaDesignDB.h>

namespace cbag {

using term_type = oa::oaTermTypeEnum;
constexpr term_type trmInput = term_type::oacInputTermType;
constexpr term_type trmOutput = term_type::oacOutputTermType;
constexpr term_type trmInout = term_type::oacInputOutputTermType;

} // namespace cbag

#else

namespace cbag {

enum term_type { input, output, inout };
constexpr term_type trmInput = term_type::input;
constexpr term_type trmOutput = term_type::output;
constexpr term_type trmInout = term_type::inout;

} // namespace cbag

#endif
#endif
