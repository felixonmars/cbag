/** \file figures.h
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#ifndef CBAG_SCHEMATIC_PIN_OBJECT_H
#define CBAG_SCHEMATIC_PIN_OBJECT_H

#include <cbag/schematic/instance.h>
#include <cbag/schematic/term_attr.h>

namespace cbag {
namespace sch {

/** A schematic object that can represent a pin.
 *
 *  In OpenAccess schematics, pins are represented using an SchInstance and an
 * AttrDisplay. This object encapsulates those two.
 */
struct pin_object {
  public:
    instance inst{};
    term_attr attr{};

    pin_object();

    /** Create an instance with empty parameter and terminal mappings.
     *
     * @param lib the library name.
     * @param cell the cell name.
     * @param view the view name.
     * @param xform the instance location.
     */
    pin_object(instance &&inst, term_attr &&attr);
};

} // namespace sch
} // namespace cbag

#endif
