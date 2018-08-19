/** \file shapes.h
 *  \brief This file defines various shapes used by the database.
 *
 *  \author Eric Chang
 *  \date   2018/07/19
 */
#ifndef CBAG_DATABASE_LAYOUT_GEOMETRIES_H
#define CBAG_DATABASE_LAYOUT_GEOMETRIES_H

#include <cbag/database/layout/datatypes.h>

namespace cbag {
namespace layout {

/** A class representing layout geometries on the same layer.
 */
class Geometry {
  public:
    Geometry() : rect_list(), poly90_list(), poly45_list(), poly_list(), mode(0) {}
    explicit Geometry(uint8_t mode)
        : rect_list(), poly90_list(), poly45_list(), poly_list(), mode(mode) {}

  private:
    RectSet rect_list;
    Polygon90Set poly90_list;
    Polygon45Set poly45_list;
    PolygonSet poly_list;
    uint8_t mode;
};

} // namespace layout
} // namespace cbag

#endif // CBAG_DATABASE_LAYOUT_GEOMETRIES_H
