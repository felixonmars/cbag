//
// Created by Eric Chang on 6/9/18.
//

#ifndef CXBASE_LAYTOUT_TEMPLATES_H
#define CXBASE_LAYTOUT_TEMPLATES_H

#include <cxbase/layout/objects.h>

namespace cxbase {
    class CTemplate {
    public:
        CTemplate() = default;

        CRect *add_rect(lay_t lay, purp_t purp, coord_t x0, coord_t y0, coord_t x1,
                        coord_t y1);

        CPin *add_pin(lay_t lay, purp_t purp, coord_t x0, coord_t y0, coord_t x1,
                      coord_t y1, std::string net, std::string lbl);

        friend std::ostream &operator<<(std::ostream &os, const CTemplate &obj);

    private:
        std::list<CRect> rect_list;
        std::list<CPin> pin_list;
    };
}


#endif //CXBASE_TEMPLATES_H