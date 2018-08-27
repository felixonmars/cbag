/** \file figures.h
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#ifndef CBAG_SCHEMATIC_PIN_FIGURE_H
#define CBAG_SCHEMATIC_PIN_FIGURE_H

#include <variant>

#include <cbag/common/sig_type.h>
#include <cbag/schematic/pin_object.h>
#include <cbag/schematic/rectangle.h>

namespace cbag {
namespace sch {

using pin_fig_t = std::variant<rectangle, pin_object>;

struct pin_figure {
    inline pin_figure() : stype(stSignal) {}

    inline pin_figure(rectangle &&obj, sig_type stype) : obj(obj), stype(stype) {}

    inline pin_figure(pin_object &&obj, sig_type stype) : obj(obj), stype(stype) {}

    pin_fig_t obj;
    sig_type stype;
};

} // namespace sch
} // namespace cbag

#endif
