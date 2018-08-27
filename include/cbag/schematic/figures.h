/** \file figures.h
 *  \brief This file defines various figures used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#ifndef CBAG_SCHEMATIC_FIGURES_H
#define CBAG_SCHEMATIC_FIGURES_H

#include <variant>

#include <cbag/common/datatypes.h>
#include <cbag/common/primitives.h>
#include <cbag/schematic/shapes.h>

namespace cbag {

// references

/** An instance object
 */
struct SchInstance {
    /** Create an empty instance.
     */
    SchInstance() = default;

    /** Create an instance with empty parameter and terminal mappings.
     *
     * @param lib the library name.
     * @param cell the cell name.
     * @param view the view name.
     * @param xform the instance location.
     */
    inline SchInstance(std::string lib, std::string cell, std::string view, Transform xform,
                       BBox bbox)
        : lib_name(std::move(lib)), cell_name(std::move(cell)), view_name(std::move(view)),
          xform(std::move(xform)), bbox(std::move(bbox)), connections(), params() {}

    // methods to manipulate parameters, so Cython doesn't have to worry about
    // variants

    void clear_params();

    void set_int_param(const char *name, int value);

    void set_double_param(const char *name, double value);

    void set_bool_param(const char *name, bool value);

    void set_string_param(const char *name, const char *value);

    void update_connection(const std::string &inst_name, uint32_t inst_size, const char *term,
                           const char *net);

    void update_connection(const std::string &inst_name, const char *term, const char *net);

    void resize_nets(uint32_t old_size, uint32_t new_size);

    inline uint32_t width() const { return bbox.getWidth(); }

    inline uint32_t height() const { return bbox.getHeight(); }

    std::string lib_name, cell_name, view_name;
    Transform xform;
    BBox bbox;
    bool is_primitive;
    std::map<std::string, std::string> connections;
    ParamMap params;
};

/** A schematic object that can represent a pin.
 *
 *  In OpenAccess schematics, pins are represented using an SchInstance and an
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
    SchPinObject(SchInstance &&inst, TermAttr &&attr) : inst(inst), attr(attr){};

    SchInstance inst;
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

#endif // CBAG_SCHEMATIC_FIGURES_H
