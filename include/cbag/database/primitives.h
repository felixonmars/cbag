/** \file primitives.h
 *  \brief This file defines the shape and geometry primitives.
 *
 *  \author Eric Chang
 *  \date   2018/07/12
 */

#ifndef CBAG_DATABASE_PRIMITIVES_H
#define CBAG_DATABASE_PRIMITIVES_H

#if __has_include(<oa/oaDesignDB.h>)
// use OpenAccess primitive types
#include <cbag/database/primitives_oa.h>
#else
// use custom primitive types
#include <cbag/database/primitives_custom.h>
#endif

// common primitive types

enum BoundaryType { PR, area, snap };

#endif // CBAG_DATABASE_PRIMITIVES_H
