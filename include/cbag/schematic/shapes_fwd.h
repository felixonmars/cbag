#ifndef CBAG_SCHEMATIC_SHAPES_FWD_H
#define CBAG_SCHEMATIC_SHAPES_FWD_H

#include <variant>

namespace cbag {
struct Rect;
struct Poly;
struct Arc;
struct Donut;
struct Ellipse;
struct Line;
struct Path;
struct Text;
struct EvalText;

using Shape = std::variant<Rect, Poly, Arc, Donut, Ellipse, Line, Path, Text, EvalText>;
} // namespace cbag

#endif // CBAG_SCHEMATIC_SHAPES_FWD_H
