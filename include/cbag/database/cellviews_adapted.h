/** \file cellviews_adapted.h
 *  \brief This file adapts cellview classes to Boost random access data
 * structures.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_DATABASE_CELLVIEWS_ADAPTED_H
#define CBAG_DATABASE_CELLVIEWS_ADAPTED_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <cbag/database/cellviews.h>

BOOST_FUSION_ADAPT_STRUCT(cbag::SchCellView, lib_name, cell_name, view_name, in_terms, out_terms,
                          io_terms, shapes, instances, props, app_defs)

BOOST_FUSION_ADAPT_STRUCT(cbag::SchCellViewInfo, cell_name, in_terms, out_terms, io_terms, props)

#endif // CBAG_DATABASE_CELLVIEWS_ADAPTED_H
