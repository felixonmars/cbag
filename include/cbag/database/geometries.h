/** \file figures.h
 *  \brief This file defines various geometry classes for boost polygon.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

#ifndef CBAG_DATABASE_GEOMETRIES_H
#define CBAG_DATABASE_GEOMETRIES_H

#include <boost/polygon/polygon.hpp>

#include <cbag/database/datatypes.h>


namespace bpo = boost::polygon;

namespace cbag {

    using GPoint = bpo::point_data<coord_t>;
    using GRect = bpo::rectangle_data<coord_t>;
    using GPoly90 = bpo::polygon_90_data<coord_t>;
    using GPoly45 = bpo::polygon_45_data<coord_t>;
    using GPoly = bpo::polygon_data<coord_t>;
}

#endif //CBAG_DATABASE_GEOMETRIES_H
