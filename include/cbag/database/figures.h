/** \file figures.h
 *  \brief This file defines various figures (shapes or references) used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#ifndef CBAG_DATABASE_SHAPES_H
#define CBAG_DATABASE_SHAPES_H

#include <boost/variant.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

#include <cbag/spirit/ast.h>
#include <cbag/database/primitives.h>
#include <cbag/database/datatypes.h>


namespace bsa = cbag::spirit::ast;

namespace cbag {

    // shapes

    /** A rectangular shape
     */
    struct Rect {
        Rect() : layer(0), purpose(0), bbox() {}

        Rect(lay_t lay, purp_t purp)
                : layer(lay), purpose(purp), bbox() {}

        Rect(lay_t lay, purp_t purp, coord_t xl, coord_t yl, coord_t xh, coord_t yh)
                : layer(lay), purpose(purp), bbox(xl, yl, xh, yh) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(layer);
            ar & BOOST_SERIALIZATION_NVP(purpose);
            ar & BOOST_SERIALIZATION_NVP(bbox);
#pragma clang diagnostic pop
        }

        lay_t layer;
        purp_t purpose;
        BBox bbox;
    };

    /** A polygon shape.
     */
    struct Poly {
        Poly() : layer(0), purpose(0), points() {}

        Poly(lay_t lay, purp_t purp, uint32_t n)
                : layer(lay), purpose(purp), points(n) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(layer);
            ar & BOOST_SERIALIZATION_NVP(purpose);
            ar & BOOST_SERIALIZATION_NVP(points);
#pragma clang diagnostic pop
        }

        lay_t layer;
        purp_t purpose;
        PointArray points;
    };

    struct Arc {
        Arc() : layer(0), purpose(0), ang_start(0), ang_stop(0), bbox() {}

        Arc(lay_t lay, purp_t purp, double start, double stop)
                : layer(lay), purpose(purp), ang_start(start), ang_stop(stop),
                  bbox() {}

        Arc(lay_t lay, purp_t purp, double start, double stop, coord_t xl, coord_t yl,
            coord_t xh, coord_t yh)
                : layer(lay), purpose(purp), ang_start(start), ang_stop(stop),
                  bbox(xl, yl, xh, yh) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(layer);
            ar & BOOST_SERIALIZATION_NVP(purpose);
            ar & BOOST_SERIALIZATION_NVP(ang_start);
            ar & BOOST_SERIALIZATION_NVP(ang_stop);
            ar & BOOST_SERIALIZATION_NVP(bbox);
#pragma clang diagnostic pop
        }

        lay_t layer;
        purp_t purpose;
        double ang_start, ang_stop;
        BBox bbox;
    };

    struct Donut {
        Donut() : layer(0), purpose(0), center(0, 0), radius(0), hole_radius(0) {}

        Donut(lay_t lay, purp_t purp, dist_t r, dist_t hole_r)
                : layer(lay), purpose(purp), radius(r), hole_radius(hole_r) {}


        Donut(lay_t lay, purp_t purp, dist_t r, dist_t hole_r, coord_t x, coord_t y)
                : layer(lay), purpose(purp), center(x, y), radius(r), hole_radius(hole_r) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(layer);
            ar & BOOST_SERIALIZATION_NVP(purpose);
            ar & BOOST_SERIALIZATION_NVP(center);
            ar & BOOST_SERIALIZATION_NVP(radius);
            ar & BOOST_SERIALIZATION_NVP(hole_radius);
#pragma clang diagnostic pop
        }

        lay_t layer;
        purp_t purpose;
        Point center;
        dist_t radius, hole_radius;
    };

    struct Ellipse {
        Ellipse() : layer(0), purpose(0), bbox() {}

        Ellipse(lay_t lay, purp_t purp)
                : layer(lay), purpose(purp), bbox() {}

        Ellipse(lay_t lay, purp_t purp, coord_t xl, coord_t yl, coord_t xh, coord_t yh)
                : layer(lay), purpose(purp), bbox(xl, yl, xh, yh) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(layer);
            ar & BOOST_SERIALIZATION_NVP(purpose);
            ar & BOOST_SERIALIZATION_NVP(bbox);
#pragma clang diagnostic pop
        }

        lay_t layer;
        purp_t purpose;
        BBox bbox;
    };

    struct Line {
        Line() : layer(0), purpose(0), points() {}

        Line(lay_t lay, purp_t purp, uint32_t n)
                : layer(lay), purpose(purp), points(n) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(layer);
            ar & BOOST_SERIALIZATION_NVP(purpose);
            ar & BOOST_SERIALIZATION_NVP(points);
#pragma clang diagnostic pop
        }

        lay_t layer;
        purp_t purpose;
        PointArray points;
    };

    struct Path {
        Path() : layer(0), purpose(0), width(0), points(), style(psTruncate),
                 begin_ext(0), end_ext(0) {}

        Path(lay_t lay, purp_t purp, dist_t width, uint32_t n,
             PathStyle style, dist_t begin_ext, dist_t end_ext)
                : layer(lay), purpose(purp), width(width), points(n), style(style),
                  begin_ext(begin_ext), end_ext(end_ext) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(layer);
            ar & BOOST_SERIALIZATION_NVP(purpose);
            ar & BOOST_SERIALIZATION_NVP(width);
            ar & BOOST_SERIALIZATION_NVP(points);
            ar & BOOST_SERIALIZATION_NVP(style);
            ar & BOOST_SERIALIZATION_NVP(begin_ext);
            ar & BOOST_SERIALIZATION_NVP(end_ext);
#pragma clang diagnostic pop
        }

        lay_t layer;
        purp_t purpose;
        dist_t width;
        PointArray points;
        PathStyle style;
        dist_t begin_ext, end_ext;
    };

    /** Base class for all text-like object.
     */
    struct TextBase {
        TextBase() : layer(0), purpose(0), origin(0, 0), alignment(taCC),
                     orient(oR0), font(fRoman), height(0), overbar(false),
                     visible(true), drafting(true) {}

        TextBase(lay_t lay, purp_t purp, TextAlign align, Orientation orient,
                 Font font, dist_t height, bool overbar, bool visible, bool drafting)
                : layer(lay), purpose(purp), origin(), alignment(align), orient(orient),
                  font(font), height(height), overbar(overbar), visible(visible),
                  drafting(drafting) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(layer);
            ar & BOOST_SERIALIZATION_NVP(purpose);
            ar & BOOST_SERIALIZATION_NVP(origin);
            ar & BOOST_SERIALIZATION_NVP(alignment);
            ar & BOOST_SERIALIZATION_NVP(orient);
            ar & BOOST_SERIALIZATION_NVP(font);
            ar & BOOST_SERIALIZATION_NVP(height);
            ar & BOOST_SERIALIZATION_NVP(overbar);
            ar & BOOST_SERIALIZATION_NVP(visible);
            ar & BOOST_SERIALIZATION_NVP(drafting);
#pragma clang diagnostic pop
        }


        lay_t layer;
        purp_t purpose;
        Point origin;
        TextAlign alignment;
        Orientation orient;
        Font font;
        dist_t height;
        bool overbar, visible, drafting;
    };

    struct Text : TextBase {
        Text() : TextBase(), text() {}

        Text(lay_t lay, purp_t purp, std::string text, TextAlign align,
             Orientation orient, Font font, dist_t height, bool overbar,
             bool visible, bool drafting)
                : TextBase(lay, purp, align, orient, font, height, overbar, visible, drafting),
                  text(std::move(text)) {}

        // boost serialization
#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TextBase);
            ar & BOOST_SERIALIZATION_NVP(text);
#pragma clang diagnostic pop
        }

#pragma clang diagnostic pop

        std::string text;
    };

    struct EvalText : Text {
        EvalText() : Text(), evaluator() {}

        EvalText(lay_t lay, purp_t purp, std::string text, TextAlign align, Orientation orient,
                 Font font, dist_t height, bool overbar, bool visible,
                 bool drafting, std::string eval)
                : Text(lay, purp, std::move(text), align, orient, font, height, overbar,
                       visible, drafting), evaluator(std::move(eval)) {}

        // boost serialization
#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Text);
            ar & BOOST_SERIALIZATION_NVP(evaluator);
#pragma clang diagnostic pop
        }

#pragma clang diagnostic pop

        std::string evaluator;
    };

    struct TermAttr : TextBase {
        TermAttr() : TextBase(), attr_type(tatName), format(tdfNameValue) {}

        TermAttr(TermAttrType attr_type, lay_t lay, purp_t purp, TextAlign align,
                 Orientation orient, Font font, dist_t height, TextDispFormat format, bool overbar,
                 bool visible, bool drafting)
                : TextBase(lay, purp, align, orient, font, height, overbar, visible, drafting),
                  attr_type(attr_type), format(format) {}

        // boost serialization
#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(TextBase);
            ar & BOOST_SERIALIZATION_NVP(attr_type);
            ar & BOOST_SERIALIZATION_NVP(format);
#pragma clang diagnostic pop
        }

#pragma clang diagnostic pop

        TermAttrType attr_type;
        TextDispFormat format;
    };

    using Shape = boost::variant<Rect, Poly, Arc, Donut, Ellipse, Line, Path,
            Text, EvalText>;

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
                : lib_name(lib), cell_name(cell), view_name(view), xform(xform), connections({}),
                  params({}) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(lib_name);
            ar & BOOST_SERIALIZATION_NVP(cell_name);
            ar & BOOST_SERIALIZATION_NVP(view_name);
            ar & BOOST_SERIALIZATION_NVP(xform);
            ar & BOOST_SERIALIZATION_NVP(connections);
            ar & BOOST_SERIALIZATION_NVP(params);
#pragma clang diagnostic pop
        }

        std::string lib_name, cell_name, view_name;
        Transform xform;
        std::map<bsa::name_bit, std::vector<bsa::name_bit>> connections;
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

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(inst);
            ar & BOOST_SERIALIZATION_NVP(attr);
#pragma clang diagnostic pop
        }

        Instance inst;
        TermAttr attr;
    };

    // figures
    struct PinFigure {
        PinFigure(Rect &&obj, SigType sig_type) : obj(obj), sig_type(sig_type) {}

        PinFigure(SchPinObject &&obj, SigType sig_type) : obj(obj), sig_type(sig_type) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(obj);
            ar & BOOST_SERIALIZATION_NVP(sig_type);
#pragma clang diagnostic pop
        }

        boost::variant<Rect, SchPinObject> obj;
        SigType sig_type;
    };
}

#endif //CBAG_DATABASE_SHAPES_H
