/*
Copyright 2008 Intel Corporation

Use, modification and distribution are subject to the Boost Software License,
Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt).
*/
#include <boost/polygon/polygon.hpp>
#include <cassert>

namespace gtl = boost::polygon;
using namespace boost::polygon::operators;

//just a little meta-programming to get things off on the right foot
template <typename T>
struct lookup_polygon_set_type { typedef gtl::polygon_set_data<int> type; };
template <typename T, typename T2>
struct lookup_polygon_set_type<gtl::property_merge_90<T, T2> > {
    typedef gtl::polygon_90_set_data<int> type; };

//This function works with both the 90 and general versions
//of property merge/map overlay algorithm
template <typename pm_type>
void test_pm() {
    std::vector<gtl::rectangle_data<int> > test_data;
    test_data.emplace_back(gtl::rectangle_data<int>(11, 10, 31, 30));
    test_data.emplace_back(gtl::rectangle_data<int>(1, 0, 21, 20));
    test_data.emplace_back(gtl::rectangle_data<int>(6, 15, 16, 25));

    pm_type pm;

    //insert our test geometry into the property merge algorithm
    for(unsigned int i = 0; i < test_data.size(); ++i) {
        pm.insert(test_data[i], i); //notice I use the index as the property value
    }

    typedef typename lookup_polygon_set_type<pm_type>::type polygon_set_type;
    typedef std::map<std::set<int>, polygon_set_type> property_merge_result_type;

    //There are 8 different combinations of our input geometries
    //null combination is not interesting, so really 7

    property_merge_result_type result;
    pm.merge(result);

    //lets enumerate boolean combinations of inputs (hold onto your hats)
    for(unsigned int i = 0; i < 8; ++i) {
        unsigned int bits[3] = {i & 1, i & 2, i & 4}; //break out bit array
        polygon_set_type test_set;
        std::set<int> key;
        for(unsigned int j = 0; j < 3; ++j) {
            if(bits[j]) {
                key.insert(key.end(), j);
                test_set += test_data[j];
            }
        }

        for(unsigned int j = 0; j < 3; ++j) {
            if(bits[j]) {
                test_set *= test_data[j];
            }
        }
        for(unsigned int j = 0; j < 3; ++j) {
            if(!bits[j])
                test_set -= test_data[j];
        }
        if(test_set.empty()) {
            //only the null combination should not exist
            assert(i == 0);
            //a combination that does not exist should not
            //be present in result
            assert(result.find(key) == result.end());
        } else {
            assert(gtl::equivalence(result[key], test_set));
        }
    }

    //Notice that we have to do O(2^n) booleans to compose the same
    //result that is produced in one pass of property merge
    //given n input layers (8 = 2^3 in this example)
}

int main() {
    test_pm<gtl::property_merge_90<int, int> >();
    test_pm<gtl::property_merge<int, int> >();
    return 0;
}
//Now you know how to use the manhattan and arbitrary angle property
//merge algorithms to perform map overlay on n layers of input geometry
