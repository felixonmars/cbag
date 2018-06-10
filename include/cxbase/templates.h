//
// Created by Eric Chang on 6/9/18.
//

#ifndef CXBASE_TEMPLATES_H
#define CXBASE_TEMPLATES_H

#include <boost/polygon/polygon.hpp>
#include <cxbase/objects.h>

namespace cxbase {
    class CTemplate {
    public:
        CTemplate() = default;

        CRect *add_rect(lay_type lay, purp_type purp, coord_type x0, coord_type y0, coord_type x1,
                        coord_type y1);

        CPin *add_pin(lay_type lay, purp_type purp, coord_type x0, coord_type y0, coord_type x1,
                      coord_type y1, std::string net, std::string lbl);

        friend std::ostream &operator<<(std::ostream &os, const CTemplate &obj);

    private:
        std::list<CRect> rect_list;
        std::list<CPin> pin_list;
    };
}


#endif //CXBASE_TEMPLATES_H
