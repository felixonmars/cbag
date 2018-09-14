
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

struct pt_vector::helper {

    static inline void *get_void_ptr(const pt_vector &self) {
        return self.points[0].operator void *();
    }
    static inline value_type *get_ptr(const pt_vector &self) {
        return self.points[0].operator point_t *();
    }
    static inline const value_type *get_const_ptr(const pt_vector &self) {
        return self.points[0].operator const point_t *();
    }
    static inline size_type get_ptr_size(const pt_vector &self) {
        return self.points[STACK_SIZE - 1].ux();
    }
    static inline size_type get_ptr_cap(const pt_vector &self) {
        return self.points[STACK_SIZE - 1].uy();
    }

    static inline void set_ptr(pt_vector &self, value_type *ptr) { self.points[0].set(ptr); }

    static inline void set_size(pt_vector &self, size_type size) {
        self.points[STACK_SIZE - 1][0] = size;
    }

    static inline void set_size_cap(pt_vector &self, size_type size, size_type cap) {
        self.points[STACK_SIZE - 1].set(size, cap);
    }

    static inline value_type *new_ptr(size_type n) {
        auto *ans = reinterpret_cast<value_type *>(malloc(n * sizeof(value_type)));
        if (ans == nullptr) {
            throw bad_alloc_pt_vector();
        }
        return ans;
    }
    static inline void reserve_ptr(pt_vector &self, size_type n) {
        void *old_ptr = get_void_ptr(self);
        void *new_ptr = realloc(old_ptr, n * sizeof(value_type));
        if (new_ptr == nullptr) {
            free(old_ptr);
            throw bad_alloc_pt_vector();
        }
        self.points[0].set(new_ptr);
        self.points[STACK_SIZE - 1][1] = n;
    }
    static inline void reserve_no_ptr(pt_vector &self, size_type n, size_type copy_size,
                                      size_type new_size) {
        auto *ptr = new_ptr(n);
        if (copy_size > 0) {
            std::copy(self.points, self.points + copy_size, ptr);
        }
        set_ptr(self, ptr);
        set_size_cap(self, new_size, n);
        self.mode = STACK_SIZE + 1;
    }
};

pt_vector::~pt_vector() noexcept {
    if (mode > STACK_SIZE) {
        free(helper::get_ptr(*this));
    }
}

pt_vector::pt_vector() noexcept = default;

pt_vector::pt_vector(const pt_vector &other) : mode(other.mode) {
    if (mode > STACK_SIZE) {
        // create new array and copy data over
        size_type n = helper::get_ptr_size(other);
        auto *dest = helper::new_ptr(n);
        auto *other_ptr = helper::get_ptr(other);
        std::copy(other_ptr, other_ptr + n, dest);
        helper::set_ptr(*this, dest);
        helper::set_size_cap(*this, n, n);
    } else {
        std::copy(other.points, other.points + mode, points);
    }
}

pt_vector &pt_vector::operator=(pt_vector other) noexcept {
    swap(*this, other);
    return *this;
}

pt_vector::pt_vector(pt_vector &&other) noexcept : pt_vector() {
    swap(*this, other);
}

void swap(pt_vector &first, pt_vector &second) {
    using std::swap;
    swap(first.mode, second.mode);
    swap(first.points, second.points);
}

pt_vector::pt_vector(size_type n) {
    if (n > STACK_SIZE) {
        helper::reserve_no_ptr(*this, n, 0, 0);
    }
}

pt_vector::pt_vector(coord_t x0, coord_t y0, coord_t x1, coord_t y1) noexcept : mode(2) {
    points[0].set(x0, y0);
    points[1].set(x1, y1);
}

pt_vector::iterator pt_vector::begin() {
    return (mode > STACK_SIZE) ? helper::get_ptr(*this) : points;
}

pt_vector::const_iterator pt_vector::begin() const {
    return (mode > STACK_SIZE) ? helper::get_const_ptr(*this) : points;
}

pt_vector::iterator pt_vector::end() {
    return (mode > STACK_SIZE) ? helper::get_ptr(*this) + helper::get_ptr_size(*this)
                               : points + mode;
}

pt_vector::const_iterator pt_vector::end() const {
    return (mode > STACK_SIZE) ? helper::get_const_ptr(*this) + helper::get_ptr_size(*this)
                               : points + mode;
}

pt_vector::size_type pt_vector::size() const {
    return (mode > STACK_SIZE) ? helper::get_ptr_size(*this) : mode;
}

const pt_vector::value_type &pt_vector::operator[](size_type idx) const { return begin()[idx]; }

pt_vector::value_type &pt_vector::operator[](size_type idx) { return begin()[idx]; }

pt_vector::value_type &pt_vector::emplace_back(coord_t x, coord_t y) {
    if (mode > STACK_SIZE) {
        size_type n = helper::get_ptr_size(*this);
        size_type cap = helper::get_ptr_cap(*this);
        if (n >= cap) {
            helper::reserve_ptr(*this, cap * 2);
        }
        helper::set_size(*this, n + 1);
        auto *ptr = helper::get_ptr(*this) + n;
        ptr->set(x, y);
        return *ptr;
    } else if (mode < STACK_SIZE) {
        points[mode].set(x, y);
        ++mode;
        return points[mode];
    } else {
        helper::reserve_no_ptr(*this, INIT_HEAP_SIZE, STACK_SIZE, STACK_SIZE + 1);
        auto *ptr = helper::get_ptr(*this) + STACK_SIZE;
        ptr->set(x, y);
        return *ptr;
    }
}

void pt_vector::reserve(size_type size) {
    if (mode > STACK_SIZE) {
        // has memory already
        if (size > helper::get_ptr_cap(*this)) {
            helper::reserve_ptr(*this, size);
        }
    } else {
        // no memory
        if (size > STACK_SIZE) {
            helper::reserve_no_ptr(*this, size, mode, mode);
        }
    }
}

void pt_vector::clear() {
    if (mode > STACK_SIZE) {
        helper::set_size(*this, 0);
    } else {
        mode = 0;
    }
}

void pt_vector::pop_back() {
    if (mode > STACK_SIZE) {
        helper::set_size(*this, size() - 1);
    } else {
        --mode;
    }
}

} // namespace layout
} // namespace cbag
