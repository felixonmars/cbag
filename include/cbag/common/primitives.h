/** \file primitives.h
 *  \brief This file defines the shape and geometry primitives.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_COMMON_PRIMITIVES_H
#define CBAG_COMMON_PRIMITIVES_H

#if __has_include(<oa/oaDesignDB.h>)
// use OpenAccess primitive types
#include <cbag/common/primitives_oa.h>
#else
// use custom primitive types
#include <cbag/common/primitives_custom.h>
#endif

namespace cbag {
// common primitive types

enum BoundaryType { PR, area, snap };
} // namespace cbag
#endif // CBAG_COMMON_PRIMITIVES_H
