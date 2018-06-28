//
// Created by erichang on 6/10/18.
//

#include <cxbase/layout/objects.h>

namespace cxbase {
    std::ostream &operator<<(std::ostream &os, const CBox &obj) {
        os << "CBox(" << obj.xl << ", " << obj.yl << ", " << obj.xh << ", " << obj.yh << ")";
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const CRect &obj) {
        os << "CRect(lay=" << obj.layer << ", purp=" << obj.purpose << ", " << obj.xl << ", "
           << obj.yl << ", " << obj.xh << ", " << obj.yh << ")";
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const CPin &obj) {
        os << "CPin(lay=" << obj.layer << ", purp=" << obj.purpose << ", net=" << obj.net
           << ", lbl=" << obj.label << ", " << obj.xl << ", " << obj.yl << ", " << obj.xh
           << ", " << obj.yh << ")";
        return os;
    }
}
