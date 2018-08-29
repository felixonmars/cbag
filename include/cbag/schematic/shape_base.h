/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_SHAPE_BASE_H
#define CBAG_SCHEMATIC_SHAPE_BASE_H

#include <string>

#include <cbag/common/typedefs.h>

namespace cbag {
namespace sch {

struct shape_base {
  public:
    lay_t layer = 0;
    purp_t purpose = 0;
    std::string net{};

    shape_base();

    shape_base(lay_t lay, purp_t purp, std::string net);
};

} // namespace sch
} // namespace cbag

#endif
