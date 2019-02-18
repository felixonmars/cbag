/** \file figures.h
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#ifndef CBAG_SCHEMATIC_PIN_FIGURE_H
#define CBAG_SCHEMATIC_PIN_FIGURE_H

#include <variant>

#include <cbag/enum/sig_type.h>
#include <cbag/enum/term_type.h>
#include <cbag/schematic/pin_object.h>
#include <cbag/schematic/rectangle.h>
#include <cbag/util/sorted_map.h>

namespace cbag {
namespace sch {

using pin_fig_t = std::variant<rectangle, pin_object>;

struct pin_figure {
  public:
    pin_fig_t obj;
    sig_type stype = sig_type::signal;
    term_type ttype = term_type::inout;
    util::sorted_map<std::string, std::string> attrs;

    pin_figure();

    pin_figure(rectangle &&obj, sig_type stype, term_type ttype);

    pin_figure(pin_object &&obj, sig_type stype, term_type ttype);
};

} // namespace sch
} // namespace cbag

#endif
