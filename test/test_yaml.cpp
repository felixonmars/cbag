
#include <iostream>

#include <boost/fusion/adapted.hpp>

#include <cbag/yaml_fusion.h>

struct One {
    One() :
            two(0),
            three(0.0),
            four(false) {}

    One(int a, double b, bool c) :
            two(a),
            three(b),
            four(c) {}

    int two;
    double three;
    bool four;
};

struct Five {
    typedef std::map<int, One> Map_t;
    Map_t six;

    typedef std::vector<One> Vector_t;
    Vector_t seven;

    std::string eight;

    typedef std::vector<std::vector<double> > Matrix_t;
    Matrix_t nine;
};

BOOST_FUSION_ADAPT_STRUCT(One, (int, two)(double, three)(bool, four))

BOOST_FUSION_ADAPT_STRUCT(Five,
                          (Five::Map_t, six)
                                  (Five::Vector_t, seven)
                                  (std::string, eight)
                                  (Five::Matrix_t, nine)
)

int main(int argc, char *argv[]) {
    Five v;
    v.six.insert(std::make_pair(123, One(3, 6.6, true)));
    v.six.insert(std::make_pair(456, One(4, 8.8, false)));

    v.seven.push_back(One(5, 1.1, true));
    v.seven.push_back(One(5, 2.2, true));

    v.eight = "eight";

    std::vector<double> temp;
    temp.push_back(2.3);
    temp.push_back(4.5);

    v.nine.push_back(temp);
    v.nine.push_back(temp);

    std::cout << yaml::serialization::to_yaml(v) << std::endl;

    ///////////////////////////////////////////
    std::cout << "==============" << std::endl;

    std::stringstream ss;

    ss << "six:            \n"
       << "  234:          \n"
       << "    two: 4      \n"
       << "    three: 5.5  \n"
       << "    four: true  \n"
       << "  345:          \n"
       << "    two: 6      \n"
       << "    three: 7.7  \n"
       << "    four: false \n"
       << "seven:          \n"
       << "  - two: 8      \n"
       << "    three: 9.9  \n"
       << "    four: true  \n"
       << "  - two: 2      \n"
       << "    three: 3.3  \n"
       << "    four: true  \n"
       << "eight: nine     \n"
       << "nine:           \n"
       << "  -             \n"
       << "    - 3.4       \n"
       << "    - 5.6       \n"
       << "  -             \n"
       << "    - 7.8       \n"
       << "    - 9.0       \n";

    yaml::serialization::from_yaml(ss.str(), v);
    std::cout << yaml::serialization::to_yaml(v) << std::endl;
}
