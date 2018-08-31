
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
    if (mode > STACK_SIZE) {
        free(points[0].operator void *());
    }
}

pt_vector::pt_vector() noexcept = default;

pt_vector::pt_vector(const pt_vector &other) : mode(other.mode) {
    if (mode > STACK_SIZE) {
        // create new array and copy data over
        size_type n = points[STACK_SIZE - 1].ux();
        void *dest = malloc(n * sizeof(point_t));
        if (dest == nullptr) {
            throw bad_alloc_pt_vector();
        }
        memcpy(dest, other.points[0].operator void *(), n * sizeof(point_t));
        points[0].set(dest);
        points[STACK_SIZE - 1].set(n, n);
    } else {
        memcpy(points, other.points, STACK_SIZE * sizeof(point_t));
    }
}

pt_vector &pt_vector::operator=(const pt_vector &other) {
    if (mode <= STACK_SIZE) {
        if (other.mode <= STACK_SIZE) {
            // no heap to no heap; direct copy
            memcpy(points, other.points, STACK_SIZE * sizeof(point_t));
        } else {
            // no heap to heap; need to allocate memory on heap
            size_type n = other.points[STACK_SIZE - 1].ux();
            void *dest = malloc(n * sizeof(point_t));
            if (dest == nullptr) {
                throw bad_alloc_pt_vector();
            }
            memcpy(dest, other.points[0].operator void *(), n * sizeof(point_t));
            points[0].set(dest);
            points[STACK_SIZE - 1].set(n, n);
        }
    } else {
        if (other.mode <= STACK_SIZE) {
            // heap to no heap; free existing memory, then direct copy
            free(points[0].operator void *());
            memcpy(points, other.points, STACK_SIZE * sizeof(point_t));
        } else {
            // heap to heap; memory reallocation depends on capacity
            size_type n_new = other.points[STACK_SIZE - 1].ux();
            size_type cap = points[STACK_SIZE - 1].uy();
            void *src = other.points[0].operator void *();
            void *dest = points[0].operator void *();
            if (n_new <= cap) {
                // capacity good, just copy memory
                memcpy(dest, src, n_new * sizeof(point_t));
                points[STACK_SIZE - 1][0] = n_new;
            } else {
                // capacity bad, reallocate memory
                free(dest);
                dest = malloc(n_new * sizeof(point_t));
                if (dest == nullptr) {
                    throw bad_alloc_pt_vector();
                }
                memcpy(dest, src, n_new * sizeof(point_t));
                points[0].set(dest);
                points[STACK_SIZE - 1].set(n_new, n_new);
            }
        }
    }
    mode = other.mode;
    return *this;
}

pt_vector::pt_vector(pt_vector &&other) noexcept : mode(other.mode) {
    memcpy(points, other.points, STACK_SIZE * sizeof(point_t));
    other.mode = 0;
}

pt_vector &pt_vector::operator=(pt_vector &&other) noexcept {
    if (mode > STACK_SIZE) {
        // free existing memory
        free(points[0].operator void *());
    }
    mode = other.mode;
    memcpy(points, other.points, STACK_SIZE * sizeof(point_t));
    other.mode = 0;
    return *this;
}

pt_vector::pt_vector(size_type n) {
    if (n > STACK_SIZE) {
        // allocate memory
        void *ptr = malloc(n * sizeof(point_t));
        if (ptr == nullptr) {
            throw bad_alloc_pt_vector();
        }
        mode = STACK_SIZE + 1;
        points[0].set(ptr);
        points[STACK_SIZE - 1].set(0u, n);
    }
}

pt_vector::pt_vector(coord_t x0, coord_t y0, coord_t x1, coord_t y1) noexcept : mode(2) {
    points[0].set(x0, y0);
    points[1].set(x1, y1);
}

pt_vector::iterator pt_vector::begin() {
    return (mode > STACK_SIZE) ? reinterpret_cast<pt_vector::iterator>(points[0].operator void *())
                               : points;
}

pt_vector::const_iterator pt_vector::begin() const {
    return (mode > STACK_SIZE)
               ? reinterpret_cast<pt_vector::const_iterator>(points[0].operator void *())
               : points;
}

pt_vector::iterator pt_vector::end() { return begin() + size(); }

pt_vector::const_iterator pt_vector::end() const { return begin() + size(); }

pt_vector::size_type pt_vector::size() const {
    return (mode > STACK_SIZE) ? points[STACK_SIZE - 1].ux() : mode;
}

const pt_vector::value_type &pt_vector::operator[](size_type idx) const { return begin()[idx]; }

pt_vector::value_type &pt_vector::operator[](size_type idx) { return begin()[idx]; }

pt_vector::value_type &pt_vector::emplace_back(coord_t x, coord_t y) {
    if (mode > STACK_SIZE) {
        size_type n = points[STACK_SIZE - 1].ux();
        size_type cap = points[STACK_SIZE - 1].uy();
        void *ptr = points[0].operator void *();
        if (n < cap) {
            points[STACK_SIZE - 1][0] = n + 1;
            auto *cur_ptr = reinterpret_cast<pt_vector::iterator>(ptr) + n;
            cur_ptr->set(x, y);
            return *cur_ptr;
        } else {
            cap *= 2;
            void *new_ptr = realloc(ptr, cap * sizeof(point_t));
            if (new_ptr == nullptr) {
                free(ptr);
                throw bad_alloc_pt_vector();
            }
            points[0].set(new_ptr);
            points[STACK_SIZE - 1].set(n + 1, cap);
            auto *cur_ptr = reinterpret_cast<pt_vector::iterator>(new_ptr) + n;
            cur_ptr->set(x, y);
            return *cur_ptr;
        }
    } else if (mode < STACK_SIZE) {
        points[mode].set(x, y);
        ++mode;
        return points[mode];
    } else {
        void *ptr = malloc(INIT_HEAP_SIZE * sizeof(point_t));
        if (ptr == nullptr) {
            throw bad_alloc_pt_vector();
        }
        mode = STACK_SIZE + 1;
        memcpy(ptr, points, STACK_SIZE * sizeof(point_t));
        points[0].set(ptr);
        points[STACK_SIZE - 1].set(STACK_SIZE + 1, INIT_HEAP_SIZE);
        auto *cur_ptr = reinterpret_cast<pt_vector::iterator>(ptr) + STACK_SIZE;
        cur_ptr->set(x, y);
        return *cur_ptr;
    }
}

void pt_vector::reserve(size_type size) {
    if (mode > STACK_SIZE) {
        // has memory already
        if (size > points[STACK_SIZE - 1].uy()) {
            // reallocate memory
            void *old_ptr = points[0].operator void *();
            void *new_ptr = realloc(old_ptr, size * sizeof(point_t));
            if (new_ptr == nullptr) {
                free(old_ptr);
                throw bad_alloc_pt_vector();
            }
            points[0].set(new_ptr);
            points[STACK_SIZE - 1][1] = size;
        }
    } else {
        // no memory
        if (size > STACK_SIZE) {
            // need to allocate memory
            void *ptr = malloc(size * sizeof(point_t));
            if (ptr == nullptr) {
                throw bad_alloc_pt_vector();
            }
            memcpy(ptr, points, mode * sizeof(point_t));
            points[0].set(ptr);
            points[STACK_SIZE - 1].set((size_type)mode, size);
            mode = STACK_SIZE + 1;
        }
    }
}

void pt_vector::clear() {
    if (mode > STACK_SIZE) {
        points[STACK_SIZE - 1][0] = 0;
    } else {
        mode = 0;
    }
}

} // namespace layout
} // namespace cbag
