#include <iostream>
#include <vector>

#include <boost/polygon/polygon.hpp>

#include <cbag/layout/blockage.h>
#include <cbag/layout/polygon90.h>
#include <cbag/layout/pt_vector.h>

namespace bp = boost::polygon;
namespace bl = cbag::layout;

using bp_point_t = bp::point_data<int32_t>;
using bp_poly90 = bp::polygon_90_data<int32_t>;

template <typename T> void print_poly(T &data) {
    std::cout << "points:" << std::endl;
    for (auto const &p : data) {
        std::cout << p.x() << " " << p.y() << std::endl;
    }
    std::cout << "done" << std::endl;
}

template <typename T> void print_poly90(T &data) {
    std::cout << "coords:" << std::endl;
    for (auto iter = data.begin_compact(); iter < data.end_compact(); ++iter) {
        std::cout << *iter << std::endl;
    }
    std::cout << "done" << std::endl;
}


void test_rect_pt_vector() {
    std::vector<bp_point_t> point_list = {
        {0, 0}, {6, 0}, {6, 2}, {2, 2}, {2, 5}, {-2, 5}, {-2, 3}, {0, 3},
    };

    bp_poly90 data;
    data.set(point_list.begin(), point_list.end());

    std::cout << "boost:" << std::endl;
    print_poly(data);
    print_poly90(data);

    bl::pt_vector pt_vec;
    for (std::size_t idx = 0; idx < point_list.size(); idx += 2) {
        pt_vec.emplace_back(point_list[idx].x(), point_list[idx].y());
    }

    std::cout << "pt_vector:" << std::endl;
    for (auto const &p : pt_vec) {
        std::cout << "x: " << p.x() << ", y: " << p.y() << std::endl;
    }
    std::cout << "done" << std::endl;

    bl::polygon90 poly(pt_vec);
    std::cout << "cbag size : " << poly.size() << std::endl;
    print_poly(poly);
    print_poly90(poly);
}

void test_subclass_constructor() {
    std::vector<bp_point_t> point_list = {
        {0, 0}, {6, 0}, {6, 2}, {2, 2}, {2, 5}, {-2, 5}, {-2, 3}, {0, 3},
    };

    bl::pt_vector pt_vec;
    for (std::size_t idx = 0; idx < point_list.size(); idx += 2) {
        pt_vec.emplace_back(point_list[idx].x(), point_list[idx].y());
    }

    std::cout << "pt_vector:" << std::endl;
    for (auto const &p : pt_vec) {
        std::cout << "x: " << p.x() << ", y: " << p.y() << std::endl;
    }
    std::cout << "done" << std::endl;

    // bl::blockage poly(std::move(pt_vec), cbag::blkRoute);
    bl::blockage poly(pt_vec, cbag::blkRoute);
    std::cout << "blockage size : " << poly.size() << std::endl;
    print_poly(poly);
}

int main(int argc, char *argv[]) {
    test_rect_pt_vector();
    // test_subclass_constructor();
    return 0;
}
