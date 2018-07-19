/** \file figures.h
 *  \brief This file defines various figures (shapes or references) used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#ifndef CBAG_DATABASE_FIGURES_H
#define CBAG_DATABASE_FIGURES_H

#include <boost/variant.hpp>

#include <cbag/database/primitives.h>
#include <cbag/database/datatypes.h>
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
                : lib_name(lib), cell_name(cell), view_name(view), xform(xform), connections(),
                  params() {}

        std::string lib_name, cell_name, view_name;
        Transform xform;
        std::map<std::string, std::string> connections;
        ParamMap params;
    };

    /** A schematic object that can represent a pin.
     *
     *  In OpenAccess schematics, pins are represented using an Instance and an AttrDisplay.
     *  This object encapsulates those two.
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
        SchPinObject(Instance &&inst, TermAttr &&attr) : inst(inst), attr(attr) {};

        Instance inst;
        TermAttr attr;
    };

    // figures
    struct PinFigure {
        PinFigure() : sig_type(stSignal) {}

        PinFigure(Rect &&obj, SigType sig_type) : obj(obj), sig_type(sig_type) {}

        PinFigure(SchPinObject &&obj, SigType sig_type) : obj(obj), sig_type(sig_type) {}

        boost::variant<Rect, SchPinObject> obj;
        SigType sig_type;
    };
}

#endif //CBAG_DATABASE_FIGURES_H
