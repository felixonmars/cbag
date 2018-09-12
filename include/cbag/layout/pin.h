/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_LAYOUT_PIN_H
#define CBAG_LAYOUT_PIN_H

#include <cbag/layout/rectangle.h>

namespace cbag {
namespace layout {

class pin : public rectangle {
  public:
    std::string net;
    std::string label;

    pin();

    pin(coord_t xl, coord_t yl, coord_t xh, coord_t yh, const char *net, const char *label);
};

} // namespace layout
} // namespace cbag

#endif
