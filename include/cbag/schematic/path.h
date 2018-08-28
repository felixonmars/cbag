/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_PATH_H
#define CBAG_SCHEMATIC_PATH_H

#include <cbag/common/path_style.h>
#include <cbag/common/point_array.h>
#include <cbag/common/typedefs.h>
#include <cbag/schematic/shape_base.h>

namespace cbag {
namespace sch {

struct path : public shape_base {
  public:
    dist_t width;
    point_array points;
    path_style style;
    dist_t begin_ext, end_ext;

    path();

    path(lay_t lay, purp_t purp, std::string net, dist_t width, uint32_t n, path_style style,
         dist_t begin_ext, dist_t end_ext);
};

} // namespace sch
} // namespace cbag

#endif
