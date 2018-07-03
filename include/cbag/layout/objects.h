//
// Created by Eric Chang on 6/8/18.
//

#ifndef CBAG_LAYOUT_OBJECTS_H
#define CBAG_LAYOUT_OBJECTS_H

#include <boost/polygon/polygon.hpp>

#include <cbag/common.h>

namespace cbag {

class CBox {
 public:
  CBox(coord_t x0, coord_t y0, coord_t x1,
       coord_t y1) : xl(x0), yl(y0), xh(x1), yh(y1) {}

  friend std::ostream &operator<<(std::ostream &os, const CBox &obj);

  coord_t xl, yl, xh, yh;
};

class CRect : public CBox {
 public:
  CRect(coord_t x0, coord_t y0, coord_t x1, coord_t y1, lay_t lay,
        purp_t purp) : CBox(x0, y0, x1, y1), layer(lay), purpose(purp) {}

  friend std::ostream &operator<<(std::ostream &os, const CRect &obj);

  lay_t layer;
  purp_t purpose;
};

class CPin : public CRect {
 public:
  CPin(coord_t x0, coord_t y0, coord_t x1, coord_t y1, lay_t lay,
       purp_t purp, std::string net, std::string lbl) : CRect(x0, y0, x1, y1, lay, purp),
                                                        net(std::move(net)),
                                                        label(std::move(lbl)) {}

  friend std::ostream &operator<<(std::ostream &os, const CPin &obj);

  std::string net;
  std::string label;
};
}

#endif //CBAG_LAYOUT_OBJECTS_H
