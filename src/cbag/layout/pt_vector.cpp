
#include <cstdlib>
#include <exception>

#include <cbag/layout/pt_vector.h>

namespace cbag {
namespace layout {

class bad_alloc_pt_vector : public std::bad_alloc {
    virtual const char *what() const noexcept override {
        return "failed to allocate memory for cbag::layout::pt_vector";
    }
};

pt_vector::pt_vector() = default;

pt_vector::pt_vector(uint32_t n) {
    if (n > 2) {
        // allocate memory
        void *ptr = malloc(n * sizeof(point_t));
        if (ptr == nullptr) {
            throw bad_alloc_pt_vector();
        }
        mode = 3;
        points[0].set(ptr);
        points[1].set(0u, n);
    }
}

pt_vector::pt_vector(coord_t x0, coord_t y0, coord_t x1, coord_t y1) : mode(2) {
    points[0].set(x0, y0);
    points[1].set(x1, y1);
}

} // namespace layout
} // namespace cbag
