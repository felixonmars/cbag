#include <iostream>

struct opt_int32_t {
    uint8_t vals[4];
};

int main(int argc, char *argv[]) {

    opt_int32_t foo;
    foo.vals[0] = 3;
    foo.vals[1] = 1;
    foo.vals[2] = 0;
    foo.vals[3] = 0;

    for (int idx = 0; idx < 4; idx++) {
        std::cout << "val " << idx << ": " << (uint32_t)foo.vals[idx] << std::endl;
    }

    std::cout << "tot val: " << *reinterpret_cast<uint32_t *>(foo.vals) << std::endl;

    int32_t a = -1;
    uint32_t b = (1 << 31);

    std::cout << a << " as uint: " << static_cast<uint32_t>(a) << std::endl;
    std::cout << b << " as sint: " << static_cast<int32_t>(b) << std::endl;
    return 0;
}
