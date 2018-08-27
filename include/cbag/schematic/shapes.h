/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_SCHEMATIC_SHAPES_H
#define CBAG_SCHEMATIC_SHAPES_H

#include <variant>

#include <cbag/common/primitives.h>
#include <cbag/schematic/shapes_fwd.h>

namespace cbag {

// shapes

struct ShapeBase {
    ShapeBase() : layer(0), purpose(0), net() {}

    ShapeBase(lay_t lay, purp_t purp, std::string net)
        : layer(lay), purpose(purp), net(std::move(net)) {}

    lay_t layer;
    purp_t purpose;
    std::string net;
};

/** A rectangular shape
 */
struct Rect : ShapeBase {
    Rect() : ShapeBase(), bbox() {}

    Rect(lay_t lay, purp_t purp, std::string net) : ShapeBase(lay, purp, std::move(net)), bbox() {}

    Rect(lay_t lay, purp_t purp, std::string net, coord_t xl, coord_t yl, coord_t xh, coord_t yh)
        : ShapeBase(lay, purp, std::move(net)), bbox(xl, yl, xh, yh) {}

    box_t bbox;
};

/** A polygon shape.
 */
struct Poly : ShapeBase {
    Poly() : ShapeBase(), points() {}

    Poly(lay_t lay, purp_t purp, std::string net, uint32_t n)
        : ShapeBase(lay, purp, std::move(net)), points(n) {}

    point_array points;
};

struct Arc : ShapeBase {
    Arc() : ShapeBase(), ang_start(0), ang_stop(0), bbox() {}

    Arc(lay_t lay, purp_t purp, std::string net, double start, double stop)
        : ShapeBase(lay, purp, std::move(net)), ang_start(start), ang_stop(stop), bbox() {}

    Arc(lay_t lay, purp_t purp, std::string net, double start, double stop, coord_t xl, coord_t yl,
        coord_t xh, coord_t yh)
        : ShapeBase(lay, purp, std::move(net)), ang_start(start), ang_stop(stop),
          bbox(xl, yl, xh, yh) {}

    double ang_start, ang_stop;
    box_t bbox;
};

struct Donut : ShapeBase {
    Donut() : ShapeBase(), center(0, 0), radius(0), hole_radius(0) {}

    Donut(lay_t lay, purp_t purp, std::string net, dist_t r, dist_t hole_r)
        : ShapeBase(lay, purp, std::move(net)), radius(r), hole_radius(hole_r) {}

    Donut(lay_t lay, purp_t purp, std::string net, dist_t r, dist_t hole_r, coord_t x, coord_t y)
        : ShapeBase(lay, purp, std::move(net)), center(x, y), radius(r), hole_radius(hole_r) {}

    point center;
    dist_t radius, hole_radius;
};

struct Ellipse : ShapeBase {
    Ellipse() : ShapeBase(), bbox() {}

    Ellipse(lay_t lay, purp_t purp, std::string net)
        : ShapeBase(lay, purp, std::move(net)), bbox() {}

    Ellipse(lay_t lay, purp_t purp, std::string net, coord_t xl, coord_t yl, coord_t xh, coord_t yh)
        : ShapeBase(lay, purp, std::move(net)), bbox(xl, yl, xh, yh) {}

    box_t bbox;
};

struct Line : ShapeBase {
    Line() : ShapeBase(), points() {}

    Line(lay_t lay, purp_t purp, std::string net, uint32_t n)
        : ShapeBase(lay, purp, std::move(net)), points(n) {}

    point_array points;
};

struct Path : ShapeBase {
    Path() : ShapeBase(), width(0), points(), style(psTruncate), begin_ext(0), end_ext(0) {}

    Path(lay_t lay, purp_t purp, std::string net, dist_t width, uint32_t n, path_style style,
         dist_t begin_ext, dist_t end_ext)
        : ShapeBase(lay, purp, std::move(net)), width(width), points(n), style(style),
          begin_ext(begin_ext), end_ext(end_ext) {}

    dist_t width;
    point_array points;
    path_style style;
    dist_t begin_ext, end_ext;
};

/** Base class for all text-like object.
 */
struct TextBase : ShapeBase {
    TextBase()
        : ShapeBase(), origin(0, 0), alignment(taCC), orient(oR0), font(fRoman), height(0),
          overbar(false), visible(true), drafting(true) {}

    TextBase(lay_t lay, purp_t purp, std::string net, text_align align, orientation orient,
             font_t font, dist_t height, bool overbar, bool visible, bool drafting)
        : ShapeBase(lay, purp, std::move(net)), origin(), alignment(align), orient(orient),
          font(font), height(height), overbar(overbar), visible(visible), drafting(drafting) {}

    point origin;
    text_align alignment;
    orientation orient;
    font_t font;
    dist_t height;
    bool overbar, visible, drafting;
};

struct Text : TextBase {
    Text() : TextBase(), text() {}

    Text(lay_t lay, purp_t purp, std::string net, std::string text, text_align align,
         orientation orient, font_t font, dist_t height, bool overbar, bool visible, bool drafting)
        : TextBase(lay, purp, std::move(net), align, orient, font, height, overbar, visible,
                   drafting),
          text(std::move(text)) {}

    std::string text;
};

struct EvalText : Text {
    EvalText() : Text(), evaluator() {}

    EvalText(lay_t lay, purp_t purp, std::string net, std::string text, text_align align,
             orientation orient, font_t font, dist_t height, bool overbar, bool visible,
             bool drafting, std::string eval)
        : Text(lay, purp, std::move(net), std::move(text), align, orient, font, height, overbar,
               visible, drafting),
          evaluator(std::move(eval)) {}

    std::string evaluator;
};

struct TermAttr : TextBase {
    TermAttr() : TextBase(), attr_type(tatName), format(tdfNameValue) {}

    TermAttr(term_attr_type attr_type, lay_t lay, purp_t purp, std::string net, text_align align,
             orientation orient, font_t font, dist_t height, text_disp_format format, bool overbar,
             bool visible, bool drafting)
        : TextBase(lay, purp, std::move(net), align, orient, font, height, overbar, visible,
                   drafting),
          attr_type(attr_type), format(format) {}

    term_attr_type attr_type;
    text_disp_format format;
};

} // namespace cbag

#endif // CBAG_SCHEMATIC_SHAPES_H
