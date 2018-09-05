# distutils: language = c++

from .util cimport rectangle, transformation, make_transform

from libcpp cimport string
from libcpp cimport pair
from libcpp cimport unordered_map
from libcpp cimport bool as cbool


ctypedef unsigned int uint8_t
ctypedef unsigned int uint32_t

cdef extern from "cbag/cbag.h" namespace "cbag" nogil:
    ctypedef unsigned int lay_t
    ctypedef unsigned int purp_t
    ctypedef int coord_t
    ctypedef int offset_t
    
    cdef void init_logging()


cdef extern from "cbag/cbag.h" namespace "cbag::layout" nogil:
    ctypedef unordered_map[string, instance] inst_map_t

    cdef cppclass polygon_ref[T]:
        polygon_ref()

        T& value()

    cdef cppclass instance:
        pass

    cdef cppclass cellview:
        cellview(string tech, uint8_t geo_mode)

        cbool empty() const

        rectangle get_bbox(lay_t lay_id, purp_t purp_id) const

        inst_map_t.iterator add_prim_instance(const char* lib, const char* cell, const char* view,
                                              const char* name, transformation xform, uint32_t nx,
                                              uint32_t ny, offset_t spx, offset_t spy)

        inst_map_t.iterator add_instance(const cellview* cv, const char* name, transformation xform,
                                         uint32_t nx, uint32_t ny, offset_t spx, offset_t spy)

        polygon_ref[rectangle] add_rect(lay_t lay_id, purp_t purp_id, coord_t xl, coord_t yl,
                                        coord_t xh, coord_t yh)

cdef class PyRect:
    cdef polygon_ref[rectangle] _ref


cdef class PyLayInstance:
    cdef inst_map_t.iterator _ptr
    cdef _master


cdef class PyLayCellView:
    cdef unique_ptr[cellview] _ptr
    cdef unicode _pin_purpose
    cdef unicode _def_purpose
    cdef _layer_map
    cdef _purpose_map
    cdef unicode _encoding
