/** \file figures.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#ifndef CBAG_DATABASE_SHAPES_H
#define CBAG_DATABASE_SHAPES_H

#include <boost/variant.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

#include <cbag/database/datatypes.h>


namespace cbag {

    /** Define the path style enum
     */
    BETTER_ENUM(PathStyle, uint32_t, truncate, extend, round, variable, chamfer, custom)

    /** Define the alignment enum
     */
    BETTER_ENUM(Alignment, uint32_t, upperLeft, centerLeft, lowerLeft, upperCenter, centerCenter,
                lowerCenter, upperRight, centerRight, lowerRight)

    /** Define the font enum
     */
    BETTER_ENUM(Font, uint32_t, euroStyle, gothic, math, roman, script, stick, fixed,
                swedish, milSpec)

    /** A point
     */
    struct Point {
        Point() : x(0), y(0) {}

        Point(coord_t x, coord_t y) : x(x), y(y) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(x);
            ar & BOOST_SERIALIZATION_NVP(y);
#pragma clang diagnostic pop
        }

        coord_t x, y;
    };

    /** A bounding box
     */
    struct BBox {
        BBox() : xl(0), yl(0), xh(0), yh(0) {}

        BBox(coord_t xl, coord_t yl, coord_t xh, coord_t yh)
                : xl(xl), yl(yl), xh(xh), yh(yh) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(xl);
            ar & BOOST_SERIALIZATION_NVP(yl);
            ar & BOOST_SERIALIZATION_NVP(xh);
            ar & BOOST_SERIALIZATION_NVP(xh);
#pragma clang diagnostic pop
        }

        coord_t xl, yl, xh, yh;
    };

    /** A rectangular shape
     */
    struct Rect {
        Rect() : layer(0), purpose(0), bbox(0, 0, 0, 0) {}

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

        Poly(lay_t lay, purp_t purp, size_t n)
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
        std::vector<Point> points;
    };

    struct Arc {
        Arc() : layer(0), purpose(0), ang_start(0), ang_stop(0), bbox(0, 0, 0, 0) {}

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

        Donut(lay_t lay, purp_t purp, coord_t x, coord_t y, dist_t r, dist_t hole_r)
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
        Ellipse() : layer(0), purpose(0), bbox(0, 0, 0, 0) {}

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

        Line(lay_t lay, purp_t purp, size_t n)
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
        std::vector<Point> points;
    };

    struct Path {
        Path() : layer(0), purpose(0), width(0), points(), style(PathStyle::truncate),
                 begin_ext(0), end_ext(0) {}

        Path(lay_t lay, purp_t purp, dist_t width, size_t n,
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
        std::vector<Point> points;
        PathStyle style;
        dist_t begin_ext, end_ext;
    };


    struct PathSeg {
        PathSeg() : layer(0), purpose(0), start(0, 0), stop(0, 0), width(0),
                    begin_style(PathStyle::truncate), end_style(PathStyle::truncate),
                    begin_ext(0), end_ext(0) {}

        PathSeg(lay_t lay, purp_t purp, coord_t x0, coord_t y0, coord_t x1, coord_t y1,
                dist_t width, PathStyle s0, PathStyle s1, dist_t begin_ext, dist_t end_ext)
                : layer(lay), purpose(purp), start(x0, y0), stop(x0, y0), width(width),
                  begin_style(s0), end_style(s1), begin_ext(begin_ext), end_ext(end_ext) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(layer);
            ar & BOOST_SERIALIZATION_NVP(purpose);
            ar & BOOST_SERIALIZATION_NVP(start);
            ar & BOOST_SERIALIZATION_NVP(stop);
            ar & BOOST_SERIALIZATION_NVP(begin_style);
            ar & BOOST_SERIALIZATION_NVP(end_style);
            ar & BOOST_SERIALIZATION_NVP(begin_ext);
            ar & BOOST_SERIALIZATION_NVP(end_ext);
#pragma clang diagnostic pop
        }

        lay_t layer;
        purp_t purpose;
        Point start, stop;
        dist_t width;
        PathStyle begin_style, end_style;
        dist_t begin_ext, end_ext;
    };

    struct Text {
        Text() : layer(0), purpose(0), text(), origin(0, 0), alignment(Alignment::centerCenter),
                 orient(Orientation::R0), font(Font::roman), height(0), overbar(false),
                 visible(true), drafting(true) {}

        Text(lay_t lay, purp_t purp, std::string text, coord_t x, coord_t y,
             Orientation orient, dist_t height)
                : layer(lay), purpose(purp), text(std::move(text)), origin(x, y),
                  alignment(Alignment::centerCenter), orient(orient), font(Font::roman),
                  height(height), overbar(false), visible(true), drafting(true) {}

        Text(lay_t lay, purp_t purp, std::string text, coord_t x, coord_t y, Alignment align,
             Orientation orient, Font font, dist_t height, bool overbar,
             bool visible, bool drafting)
                : layer(lay), purpose(purp), text(std::move(text)), origin(x, y), alignment(align),
                  orient(orient), font(font), height(height), overbar(overbar), visible(visible),
                  drafting(drafting) {}

        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & BOOST_SERIALIZATION_NVP(layer);
            ar & BOOST_SERIALIZATION_NVP(purpose);
            ar & BOOST_SERIALIZATION_NVP(text);
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
        std::string text;
        Point origin;
        Alignment alignment;
        Orientation orient;
        Font font;
        dist_t height;
        bool overbar, visible, drafting;
    };

    struct EvalText : Text {
        EvalText() : Text(), evaluator() {}

        EvalText(lay_t lay, purp_t purp, std::string text, coord_t x, coord_t y,
                 Orientation orient, dist_t height, std::string eval)
                : Text(lay, purp, std::move(text), x, y, orient, height),
                  evaluator(std::move(eval)) {}

        EvalText(lay_t lay, purp_t purp, std::string text, coord_t x, coord_t y, Alignment align,
                 Orientation orient, Font font, dist_t height, bool overbar,
                 bool visible, bool drafting, std::string eval)
                : Text(lay, purp, std::move(text), x, y, align, orient, font, height, overbar,
                       visible, drafting), evaluator(std::move(eval)) {}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"
        // boost serialization
        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-value"
            ar & boost::serialization::base_object<Text>(*this);
            ar & BOOST_SERIALIZATION_NVP(evaluator);
#pragma clang diagnostic pop
        }
#pragma clang diagnostic pop

        std::string evaluator;
    };

    using Shape = boost::variant<Rect, Poly, Arc, Donut, Ellipse, Line, Path, PathSeg,
            Text, EvalText>;
}

// Declare split serialization routines for enums

BOOST_SERIALIZATION_SPLIT_FREE(cbag::Alignment)
BOOST_SERIALIZATION_SPLIT_FREE(cbag::Font)
BOOST_SERIALIZATION_SPLIT_FREE(cbag::PathStyle)


#endif //CBAG_DATABASE_SHAPES_H
