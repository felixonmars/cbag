/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_PIN_H
#define CBAG_LAYOUT_PIN_H

#include <string>

#include <cbag/common/box_t.h>

namespace cbag {
namespace layout {

class pin : public box_t {
  public:
    std::string net;
    std::string label;

    pin();

    pin(coord_t xl, coord_t yl, coord_t xh, coord_t yh, std::string net, std::string label);
};

} // namespace layout
} // namespace cbag

#endif
