/** \file figures.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */
#ifndef CBAG_DATABASE_SHAPES_H
#define CBAG_DATABASE_SHAPES_H

#include <cbag/database/datatypes.h>
#include <cbag/database/geometries.h>


namespace cbag {

    /** Define the alignment enum
     */
    BETTER_ENUM(Alignment, uint32_t, upperLeft, centerLeft, lowerLeft, upperCenter, centerCenter, lowerCenter,
                upperRight, centerRight, lowerRight)

    /** Define the font enum
     */
    BETTER_ENUM(Font, uint32_t, euroStyle, gothic, math, roman, script, stick, fixed, swedish, milSpec)

    /** Define the path style enum
     */
    BETTER_ENUM(PathStyle, uint32_t, truncate, extend, round, variable, chamfer, custom)

    /** A rectangular shape
     */
    struct RectShape {
        RectShape() : layer(0), purpose(0), bbox(0, 0, 0, 0) {}

        RectShape(lay_t lay, purp_t purp, coord_t xl, coord_t yl, coord_t xh, coord_t yh)
                : layer(lay), purpose(purp), bbox(xl, yl, xh, yh) {}

        lay_t layer;
        purp_t purpose;
        Rect bbox;
    };

    /** A polygon shape.
     */
    struct PolyShape {
        PolyShape() : layer(0), purpose(0), poly() {}

        PolyShape(lay_t lay, purp_t purp) : layer(lay), purpose(purp), poly() {}

        lay_t layer;
        purp_t purpose;
        Poly poly;
    };

    struct Arc {
        Arc() : layer(0), purpose(0), ang_start(0), ang_stop(0), bbox(0, 0, 0, 0) {}

        Arc(lay_t lay, purp_t purp, double start, double stop, coord_t xl, coord_t yl, coord_t xh, coord_t yh)
                : layer(lay), purpose(purp), ang_start(start), ang_stop(stop), bbox(xl, yl, xh, yh) {}

        lay_t layer;
        purp_t purpose;
        double ang_start, ang_stop;
        Rect bbox;
    };

    struct Donut {
        Donut() : layer(0), purpose(0), center(0, 0), radius(0), hole_radius(0) {}

        Donut(lay_t lay, purp_t purp, coord_t x, coord_t y, dist_t r, dist_t hole_r)
                : layer(lay), purpose(purp), center(x, y), radius(r), hole_radius(hole_r) {}

        lay_t layer;
        purp_t purpose;
        Point center;
        dist_t radius, hole_radius;
    };

    struct Ellipse {
        Ellipse() : layer(0), purpose(0), bbox(0, 0, 0, 0) {}

        Ellipse(lay_t lay, purp_t purp, coord_t xl, coord_t yl, coord_t xh, coord_t yh)
                : layer(lay), purpose(purp), bbox(xl, yl, xh, yh) {}

        lay_t layer;
        purp_t purpose;
        Rect bbox;
    };

    struct Line {
        Line() : layer(0), purpose(0), point_list() {}

        Line(lay_t lay, purp_t purp, std::vector<Point>::size_type num_pts)
                : layer(lay), purpose(purp), point_list(num_pts) {}

        Line(lay_t lay, purp_t purp, std::vector<Point> &&point_list)
                : layer(lay), purpose(purp), point_list(point_list) {}

        lay_t layer;
        purp_t purpose;
        std::vector<Point> point_list;
    };

    struct Path {
        Path() : layer(0), purpose(0), width(0), point_list(), style(PathStyle::truncate), begin_ext(0), end_ext(0) {}

        Path(lay_t lay, purp_t purp, dist_t width, std::vector<Point>::size_type num_pts, PathStyle style,
             dist_t begin_ext, dist_t end_ext)
                : layer(lay), purpose(purp), width(width), point_list(num_pts), style(style),
                  begin_ext(begin_ext), end_ext(end_ext) {}

        lay_t layer;
        purp_t purpose;
        dist_t width;
        std::vector<Point> point_list;
        PathStyle style;
        dist_t begin_ext, end_ext;
    };


    struct PathSeg {
        PathSeg() : layer(0), purpose(0), start(0, 0), stop(0, 0), width(0),
                    begin_style(PathStyle::truncate), end_style(PathStyle::truncate), begin_ext(0), end_ext(0) {}

        PathSeg(lay_t lay, purp_t purp, coord_t x0, coord_t y0, coord_t x1, coord_t y1, dist_t width,
                PathStyle s0, PathStyle s1, dist_t begin_ext, dist_t end_ext)
                : layer(lay), purpose(purp), start(x0, y0), stop(x0, y0), width(width),
                  begin_style(s0), end_style(s1), begin_ext(begin_ext), end_ext(end_ext) {}

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

        Text(lay_t lay, purp_t purp, std::string text, coord_t x, coord_t y, Orientation orient, dist_t height)
                : layer(lay), purpose(purp), text(std::move(text)), origin(x, y), alignment(Alignment::centerCenter),
                  orient(orient), font(Font::roman), height(height), overbar(false), visible(true), drafting(true) {}

        Text(lay_t lay, purp_t purp, std::string text, coord_t x, coord_t y, Alignment align, Orientation orient,
             Font font, dist_t height, bool overbar, bool visible, bool drafting)
                : layer(lay), purpose(purp), text(std::move(text)), origin(x, y), alignment(align), orient(orient),
                  font(font), height(height), overbar(overbar), visible(visible), drafting(drafting) {}

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
}


#endif //CBAG_DATABASE_SHAPES_H
