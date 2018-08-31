
#include <cstdlib>
#include <cstring>
#include <exception>

#include <cbag/layout/pt_vector.h>

namespace cbag {
namespace layout {

class bad_alloc_pt_vector : public std::bad_alloc {
    virtual const char *what() const noexcept override {
        return "failed to allocate memory for cbag::layout::pt_vector";
    }
};

pt_vector::~pt_vector() noexcept {
    if (mode > 2) {
        free(points[0].operator void *());
    }
}

pt_vector::pt_vector() noexcept = default;

pt_vector::pt_vector(const pt_vector &other) : mode(other.mode) {
    if (mode > 2) {
        // create new array and copy data over
        size_type n = points[1].ux();
        void *dest = malloc(n * sizeof(point_t));
        if (dest == nullptr) {
            throw bad_alloc_pt_vector();
        }
        memcpy(dest, other.points[0].operator void *(), n * sizeof(point_t));
        points[0].set(dest);
        points[1].set(n, n);
    } else {
        points[0] = other.points[0];
        points[1] = other.points[1];
    }
}

pt_vector &pt_vector::operator=(const pt_vector &other) {
    if (mode < 3) {
        if (other.mode < 3) {
            // no heap to no heap; direct copy
            points[0] = other.points[0];
            points[1] = other.points[1];
        } else {
            // no heap to heap; need to allocate memory on heap
            size_type n = other.points[1].ux();
            void *dest = malloc(n * sizeof(point_t));
            if (dest == nullptr) {
                throw bad_alloc_pt_vector();
            }
            memcpy(dest, other.points[0].operator void *(), n * sizeof(point_t));
            points[0].set(dest);
            points[1].set(n, n);
        }
    } else {
        if (other.mode < 3) {
            // heap to no heap; free existing memory, then direct copy
            free(points[0].operator void *());
            points[0] = other.points[0];
            points[1] = other.points[1];
        } else {
            // heap to heap; memory reallocation depends on capacity
            size_type n_new = other.points[1].ux();
            size_type cap = points[1].uy();
            void *src = other.points[0].operator void *();
            void *dest = points[0].operator void *();
            if (n_new <= cap) {
                // capacity good, just copy memory
                memcpy(dest, src, n_new * sizeof(point_t));
                points[1][0] = n_new;
            } else {
                // capacity bad, reallocate memory
                free(dest);
                dest = malloc(n_new * sizeof(point_t));
                if (dest == nullptr) {
                    throw bad_alloc_pt_vector();
                }
                memcpy(dest, src, n_new * sizeof(point_t));
                points[0].set(dest);
                points[1].set(n_new, n_new);
            }
        }
    }
    mode = other.mode;
    return *this;
}

pt_vector::pt_vector(pt_vector &&other) noexcept : mode(other.mode) {
    points[0] = std::move(other.points[0]);
    points[1] = std::move(other.points[1]);
    other.mode = 0;
}

pt_vector &pt_vector::operator=(pt_vector &&other) noexcept {
    if (mode > 2) {
        // free existing memory
        free(points[0].operator void *());
    }
    mode = other.mode;
    points[0] = std::move(other.points[0]);
    points[1] = std::move(other.points[1]);
    other.mode = 0;
    return *this;
}

pt_vector::pt_vector(size_type n) {
    if (n > 2) {
        // allocate memory
        void *ptr = malloc(n * sizeof(point_t));
        if (ptr == nullptr) {
            throw bad_alloc_pt_vector();
        }
        mode = static_cast<uint8_t>(winding::UNKNOWN);
        points[0].set(ptr);
        points[1].set(0u, n);
    }
}

pt_vector::pt_vector(coord_t x0, coord_t y0, coord_t x1, coord_t y1) noexcept : mode(2) {
    points[0].set(x0, y0);
    points[1].set(x1, y1);
}

pt_vector::iterator pt_vector::begin() {
    return (mode > 2) ? reinterpret_cast<pt_vector::iterator>(points[0].operator void *()) : points;
}

pt_vector::const_iterator pt_vector::begin() const {
    return (mode > 2) ? reinterpret_cast<pt_vector::const_iterator>(points[0].operator void *())
                      : points;
}

pt_vector::iterator pt_vector::end() { return begin() + size(); }

pt_vector::const_iterator pt_vector::end() const { return begin() + size(); }

pt_vector::size_type pt_vector::size() const { return (mode > 2) ? points[1].ux() : mode; }

const pt_vector::value_type &pt_vector::operator[](size_type idx) const { return begin()[idx]; }

pt_vector::value_type &pt_vector::operator[](size_type idx) { return begin()[idx]; }

pt_vector::value_type &pt_vector::emplace_back(coord_t x, coord_t y) {
    if (mode > 2) {
        size_type n = points[1].ux();
        size_type cap = points[1].uy();
        void *ptr = points[0].operator void *();
        mode = static_cast<uint8_t>(winding::UNKNOWN);
        if (n < points[1].uy()) {
            points[1][0] = n + 1;
            auto *cur_ptr = reinterpret_cast<pt_vector::iterator>(ptr) + n;
            cur_ptr->set(x, y);
            return *cur_ptr;
        } else {
            cap *= 2;
            void *new_ptr = realloc(ptr, cap);
            if (new_ptr == nullptr) {
                free(ptr);
                throw bad_alloc_pt_vector();
            }
            points[0].set(new_ptr);
            points[1].set(n + 1, cap);
            auto *cur_ptr = reinterpret_cast<pt_vector::iterator>(new_ptr) + n;
            cur_ptr->set(x, y);
            return *cur_ptr;
        }
    } else if (mode < 2) {
        points[mode].set(x, y);
        ++mode;
        return points[mode];
    } else {
        void *ptr = malloc(INIT_SIZE * sizeof(point_t));
        if (ptr == nullptr) {
            throw bad_alloc_pt_vector();
        }
        mode = static_cast<uint8_t>(winding::UNKNOWN);
        memcpy(ptr, points, 2 * sizeof(point_t));
        points[0].set(ptr);
        points[1].set(3u, INIT_SIZE);
        auto *cur_ptr = reinterpret_cast<pt_vector::iterator>(ptr) + 2;
        cur_ptr->set(x, y);
        return *cur_ptr;
    }
}

void pt_vector::clear() {
    if (mode > 2) {
        // free existing memory
        free(points[0].operator void *());
    }
    mode = 0;
}

} // namespace layout
} // namespace cbag
