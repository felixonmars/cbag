//
// Created by erichang on 7/17/18.
//

#ifndef CBAG_DATABASE_DATATYPES_ADAPTED_H
#define CBAG_DATABASE_DATATYPES_ADAPTED_H

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <cbag/database/datatypes.h>


BOOST_FUSION_ADAPT_STRUCT(cbag::Time,
        time_val
)

BOOST_FUSION_ADAPT_STRUCT(cbag::Binary,
        name, bin_val
)

#endif //CBAG_DATABASE_DATATYPES_ADAPTED_H
