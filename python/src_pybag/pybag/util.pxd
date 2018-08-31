# distutils: language = c++

cdef extern from "cbag/cbag.h" namespace "cbag::layout" nogil:
    cdef cppclass rectangle:
        rectangle() except +
        rectangle(int xl, int yl, int xh, int yh) except +
        void set_rect(int xl, int yl, int xh, int yh)        
        
# distutils: language = c++

cdef extern from "cbag/cbag.h" namespace "cbag::layout" nogil:
    ctypedef int coord_t

    cdef cppclass transformation:
        transformation(coord_t dx, coord_t dy, unsigned int mode)

    cdef cppclass 
