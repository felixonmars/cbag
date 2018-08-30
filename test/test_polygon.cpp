#include <iostream>
#include <variant>
#include <vector>
#include <memory>
#include <string>

#include <boost/polygon/polygon.hpp>

namespace bp = boost::polygon;

using point_t = bp::point_data<int32_t>;

struct opt_ptr_t {
    uint32_t addr_u;
    uint32_t addr_l;
};

struct opt_vector {
    opt_ptr_t addr;
    uint32_t size;
    uint32_t capacity;
};

using test_type = std::variant<opt_vector, point_t[2]>;

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

    std::cout << "2 points size: " << sizeof(point_t[2]) << std::endl;
    std::cout << "point vector size: " << sizeof(opt_vector) << std::endl;
    std::cout << "variant size: " << sizeof(test_type) << std::endl;
    std::cout << "unique_ptr size: " << sizeof(std::unique_ptr<point_t>) << std::endl;
    std::cout << "shared_ptr size: " << sizeof(std::shared_ptr<point_t>) << std::endl;
    std::cout << "string size: " << sizeof(std::string) << std::endl;
    std::cout << "int32 string size: " << sizeof(std::basic_string<int32_t>) << std::endl;

    return 0;
}
