# distutils: language = c++

from libcpp cimport bool as cbool

ctypedef unsigned int uint32_t

cdef extern from "cbag/cbag.h" namespace "cbag" nogil:
    ctypedef int coord_t
    ctypedef int offset_t
    
    cdef void init_logging()


cdef extern from "cbag/cbag.h" namespace "cbag::layout" nogil:
    cdef cppclass transformation:
        transformation()

        transformation(coord_t dx, coord_t dy, uint32_t mode)

        uint32_t orient_code() const
        void get_location(coord_t& x, coord_t& y) const

        void set_location(coord_t x, coord_t y)
        void set_orient_code(uint32_t code)
        void move_by(offset_t dx, offset_t dy)
        void transform(coord_t x, coord_t y)

    cdef cppclass rectangle:
        coord_t xl
        coord_t yl
        coord_t xh
        coord_t yh

        rectangle()

        rectangle(coord_t xl, coord_t yl, coord_t xh, coord_t yh)

        coord_t xm() const
        coord_t ym() const
        offset_t w() const
        offset_t h() const

        cbool is_physical() const
        cbool is_valid() const
        cbool overlaps(const rectangle &r) const

        rectangle get_merge(const rectangle& other) const
        rectangle get_intersect(const rectangle& other) const
        rectangle get_extend_to(coord_t x, coord_t y) const
        rectangle get_transform(const transformation& xform) const

cdef transformation make_transform(loc, orient)


cdef class BBox:
    cdef rectangle _r

    cdef BBox c_transform(self, const transformation& xform)

    cpdef BBox move_by(self, int dx=?, int dy=?, unit_mode=?)

cdef class BBoxArray:
    cdef BBox _bbox
    cdef int _nx
    cdef int _ny
    cdef offset_t _spx
    cdef offset_t _spy

    cpdef BBox get_overall_bbox(self)

cdef class BBoxCollection:
    cdef _box_arr_list
