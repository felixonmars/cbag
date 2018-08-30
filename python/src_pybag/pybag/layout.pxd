# distutils: language = c++

cdef extern from "cbag/cbag.h" namespace "cbag::layout" nogil:
    cdef cppclass rectangle:
        rectangle() except +
        rectangle(int xl, int yl, int xh, int yh) except +
        void set_rect(int xl, int yl, int xh, int yh)        
        
        
cdef class BBox:
    cdef int _xl
    cdef int _yl
    cdef int _xh
    cdef int _yh
