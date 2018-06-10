//
// Created by Eric Chang on 6/8/18.
//

#ifndef CXBASE_DATATYPES_H
#define CXBASE_DATATYPES_H

#include <boost/polygon/polygon.hpp>

namespace cxbase {
    typedef int32_t coord_type;
    typedef int32_t lay_type;
    typedef int32_t purp_type;

    class CBox {
    public:
        CBox(coord_type x0, coord_type y0, coord_type x1,
             coord_type y1) : xl(x0), yl(y0), xh(x1), yh(y1) {}

        friend std::ostream &operator<<(std::ostream &os, const CBox &obj);

        coord_type xl, yl, xh, yh;
    };

    class CRect : public CBox {
    public:
        CRect(coord_type x0, coord_type y0, coord_type x1, coord_type y1, lay_type lay,
              purp_type purp) : CBox(x0, y0, x1, y1), layer(lay), purpose(purp) {}

        friend std::ostream &operator<<(std::ostream &os, const CRect &obj);

        lay_type layer;
        purp_type purpose;
    };

    class CPin : public CRect {
    public:
        CPin(coord_type x0, coord_type y0, coord_type x1, coord_type y1, lay_type lay,
             purp_type purp, std::string net, std::string lbl) : CRect(x0, y0, x1, y1, lay, purp),
                                                                 net(std::move(net)),
                                                                 label(std::move(lbl)) {}

        friend std::ostream &operator<<(std::ostream &os, const CPin &obj);

        std::string net;
        std::string label;
    };
}


#endif //CXBASE_DATATYPES_H
