# distutils: language = c++

from libcpp cimport bool as cbool


ctypedef unsigned int uint32_t

cdef extern from "cbag/cbag.h" namespace "cbag" nogil:
    ctypedef int coord_t
    
    cdef void init_logging()


cdef extern from "cbag/cbag.h" namespace "cbag::layout" nogil:
    cdef cppclass transformation:
        transformation()

        transformation(coord_t dx, coord_t dy, uint32_t mode)

    cdef cppclass rectangle:
        rectangle()

        rectangle(coord_t xl, coord_t yl, coord_t xh, coord_t yh)

        coord_t xl() const
        coord_t yl() const
        coord_t xh() const
        coord_t yh() const

        rectangle& transform(const transformation& xform)


cdef class CBox:
    cdef coord_t _xl
    cdef coord_t _yl
    cdef coord_t _xh
    cdef coord_t _yh

    cdef coord_t _xm(self)
    cdef coord_t _ym(self)
    cdef CBox _transform(self, coord_t dx, coord_t dy, uint32_t code)
