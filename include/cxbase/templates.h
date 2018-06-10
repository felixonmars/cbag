//
// Created by Eric Chang on 6/9/18.
//

#ifndef CXBASE_TEMPLATES_H
#define CXBASE_TEMPLATES_H

#include <boost/polygon/polygon.hpp>
#include "objects.h"

namespace cxbase {
    class Template {
    public:
        Template()=default;

        CRect *add_rect(lay_type lay, purp_type purp, coord_type x0, coord_type y0, coord_type x1, coord_type y1);

    private:
        std::vector<CRect> rect_list;
        std::vector<CPin> pin_list;
    };
}


#endif //CXBASE_TEMPLATES_H
