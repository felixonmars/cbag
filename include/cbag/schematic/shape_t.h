#ifndef CBAG_SCHEMATIC_SHAPE_T_H
#define CBAG_SCHEMATIC_SHAPE_T_H

#include <variant>

namespace cbag {
namespace sch {

struct rectangle;
struct polygon;
struct arc;
struct donut;
struct ellipse;
struct line;
struct path;
struct text_t;
struct eval_text;

using shape_t =
    std::variant<rectangle, polygon, arc, donut, ellipse, line, path, text_t, eval_text>;

} // namespace sch
} // namespace cbag

#endif
