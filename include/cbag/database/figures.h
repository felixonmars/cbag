/** \file figures.h
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#ifndef CBAG_DATABASE_FIGURES_H
#define CBAG_DATABASE_FIGURES_H

#include <variant>

#include <cbag/database/datatypes.h>
#include <cbag/database/primitives.h>
#include <cbag/database/shapes.h>

namespace cbag {

// references

/** An instance object
 */
struct Instance {
    /** Create an empty instance.
     */
    Instance() = default;

    /** Create an instance with empty parameter and terminal mappings.
     *
     * @param lib the library name.
     * @param cell the cell name.
     * @param view the view name.
     * @param xform the instance location.
     */
    Instance(std::string &&lib, std::string &&cell, std::string &&view, Transform xform)
        : lib_name(lib), cell_name(cell), view_name(view), xform(xform), connections(), params() {}

    // methods to manipulate parameters, so Cython doesn't have to worry about
    // variants

    void clear_params();

    void set_int_param(const char *name, int value);

    void set_double_param(const char *name, double value);

    void set_bool_param(const char *name, bool value);

    void set_string_param(const char *name, const char *value);

    void update_connection(const std::string &inst_name, uint32_t inst_size, const char *term,
                           const char *net);

    void resize_nets(uint32_t old_size, uint32_t new_size);

    std::string lib_name, cell_name, view_name;
    Transform xform;
    std::map<std::string, std::string> connections;
    ParamMap params;
};

/** A schematic object that can represent a pin.
 *
 *  In OpenAccess schematics, pins are represented using an Instance and an
 * AttrDisplay. This object encapsulates those two.
 */
struct SchPinObject {
    /** Create an empty instance.
     */
    SchPinObject() = default;

    /** Create an instance with empty parameter and terminal mappings.
     *
     * @param lib the library name.
     * @param cell the cell name.
     * @param view the view name.
     * @param xform the instance location.
     */
    SchPinObject(Instance &&inst, TermAttr &&attr) : inst(inst), attr(attr){};

    Instance inst;
    TermAttr attr;
};

// figures
struct PinFigure {
    PinFigure() : sig_type(stSignal) {}

    PinFigure(Rect &&obj, SigType sig_type) : obj(obj), sig_type(sig_type) {}

    PinFigure(SchPinObject &&obj, SigType sig_type) : obj(obj), sig_type(sig_type) {}

    std::variant<Rect, SchPinObject> obj;
    SigType sig_type;
};
} // namespace cbag

#endif // CBAG_DATABASE_FIGURES_H
