//
// Created by erichang on 7/17/18.
//

#ifndef CBAG_DATABASE_CELLVIEWS_ADAPTED_H
#define CBAG_DATABASE_CELLVIEWS_ADAPTED_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <cbag/database/cellviews.h>


BOOST_FUSION_ADAPT_STRUCT(cbag::SchCellView,
        in_terms, out_terms, io_terms, shapes, instances, props, app_defs
)

#endif //CBAG_DATABASE_CELLVIEWS_ADAPTED_H
