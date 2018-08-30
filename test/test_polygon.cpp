#include <iostream>

#include <boost/polygon/polygon.hpp>

namespace bp = boost::polygon;

int main(int argc, char *argv[]) {
    bp::point_data<int> shift(-1, 3);
    auto rotate = bp::axis_transformation(bp::axis_transformation::ATR::ROTATE_LEFT);
    auto xform = bp::transformation<int>(rotate, shift);

    bp::rectangle_data<int> rect(4, -2, 7, -1);
    std::cout << "before: [" << bp::xl(rect) << ", " << bp::yl(rect) << ", " << bp::xh(rect) << ", "
              << bp::yh(rect) << "]" << std::endl;
    bp::transform(rect, xform);
    std::cout << "after: [" << bp::xl(rect) << ", " << bp::yl(rect) << ", " << bp::xh(rect) << ", "
              << bp::yh(rect) << "]" << std::endl;

    return 0;
}
