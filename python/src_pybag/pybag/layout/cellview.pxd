# distutils: language = c++

from .util cimport rectangle

from libcpp cimport string
from libcpp cimport bool as cbool


ctypedef unsigned int uint8_t


cdef extern from "cbag/cbag.h" namespace "cbag" nogil:
    ctypedef unsigned int lay_t
    ctypedef unsigned int purp_t
    
    cdef void init_logging()


cdef extern from "cbag/cbag.h" namespace "cbag::layout" nogil:
    cdef cppclass cellview:
        cellview(string tech, uint8_t geo_mode)

        cbool empty() const

        rectangle get_bbox(lay_t lay_id, purp_t purp_id) const


cdef class PyLayCellView:
    cdef unique_ptr[cellview] _ptr
    cdef unicode _pin_purpose
    cdef unicode _def_purpose
    cdef _layer_map
    cdef _purpose_map
    cdef unicode _encoding
