//
// Created by erichang on 6/10/18.
//

#include <cxbase/layout/templates.h>

namespace cxbase {

    CRect *
    CTemplate::add_rect(lay_t lay, purp_t purp, coord_t x0, coord_t y0, coord_t x1,
                        coord_t y1) {
        rect_list.emplace_back(x0, y0, x1, y1, lay, purp);
        return &rect_list.back();
    }

    CPin *
    CTemplate::add_pin(cxbase::lay_t lay, cxbase::purp_t purp, cxbase::coord_t x0,
                       cxbase::coord_t y0, cxbase::coord_t x1, cxbase::coord_t y1,
                       std::string net, std::string lbl) {
        pin_list.emplace_back(x0, y0, x1, y1, lay, purp, net, lbl);
        return &pin_list.back();
    }

    std::ostream &operator<<(std::ostream &os, const CTemplate &obj) {
        os << "CTemplate:" << std::endl;
        for (auto const &rect : obj.rect_list) {
            os << rect << std::endl;
        }
        for (auto const &pin : obj.pin_list) {
            os << pin << std::endl;
        }
        return os;
    }

}
