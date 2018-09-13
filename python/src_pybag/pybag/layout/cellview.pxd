# distutils: language = c++

from .util cimport rectangle, transformation, BBox

from libcpp.string cimport string
from libcpp.utility cimport pair
from libcpp.unordered_map cimport unordered_map
from libcpp cimport bool as cbool
from libcpp.memory cimport unique_ptr

ctypedef unsigned int uint8_t
ctypedef unsigned int uint32_t


cdef extern from "cbag/cbag.h" namespace "cbag" nogil:
    ctypedef unsigned int lay_t
    ctypedef unsigned int purp_t
    ctypedef int coord_t
    ctypedef int offset_t
    ctypedef int dist_t
    
    cdef void init_logging()


cdef extern from "cbag/cbag.h" namespace "cbag::layout" nogil:
    ctypedef unordered_map[string, instance].iterator inst_iter_t

    cdef cppclass tech:
        tech(unordered_map[string, lay_t], unordered_map[string, purp_t],
             unordered_map[string, pair[lay_t, lay_t]], const string&, const string&, bool) except +

    cdef cppclass polygon:
        pass

    cdef cppclass via:
        pass

    cdef cppclass vector_obj_ref[T]:
        vector_obj_ref()

        T& value()

    cdef cppclass instance:
        void set_int_param(const char* name, int value)

        void set_double_param(const char* name, double value)

        void set_bool_param(const char* name, cbool value)

        void set_string_param(const char* name, const char* value)

    cdef cppclass cellview:
        string cell_name

        cellview(tech* tech_ptr, const char* cell_name, uint8_t geo_mode)

        cbool empty() const

        rectangle get_bbox(const char* layer, const char* purpose) except +

        inst_iter_t add_prim_instance(const char* lib, const char* cell, const char* view,
                                      const char* name, transformation xform, uint32_t nx,
                                      uint32_t ny, offset_t spx, offset_t spy)

        inst_iter_t add_instance(const cellview* cv, const char* name, transformation xform,
                                 uint32_t nx, uint32_t ny, offset_t spx, offset_t spy)

        vector_obj_ref[rectangle] add_rect(const char* layer, const char* purpose, coord_t xl,
                                           coord_t yl, coord_t xh, coord_t yh) except +

        void add_pin(const char *layer, coord_t xl, coord_t yl, coord_t xh, coord_t yh,
                     const char *net, const char *label) except +

        vector_obj_ref[via] add_via(transformation xform, const char* vid, const uint32_t (&num)[2],
                                    const dist_t (&cut_dim)[2], const offset_t (&cut_sp)[2],
                                    const offset_t (&lay1_enc)[2], const offset_t (&lay1_off)[2],
                                    const offset_t (&lay2_enc)[2], const offset_t (&lay2_off)[2],
                                    cbool add_layers) except +


cdef class PyTech:
    cdef unique_ptr[tech] _ptr


cdef class PyRect:
    cdef vector_obj_ref[rectangle] _ref


cdef class PyPolygon:
    cdef vector_obj_ref[polygon] _ref


cdef class PyVia:
    cdef vector_obj_ref[via] _ref


cdef class PyPath:
    pass


cdef class PyBlockage:
    pass


cdef class PyBoundary:
    pass


cdef class PyLayInstance:
    cdef inst_iter_t _ptr
    cdef _grid
    cdef _master
    cdef unicode _lib_name


cdef class PyLayCellView:
    cdef unique_ptr[cellview] _ptr
    cdef unicode _encoding

    cdef PyLayInstance _add_cinst(self, grid, master, lib_name, PyLayCellView cv, const char *inst_name,
                                  transformation xform, int nx, int ny, int spx, int spy)
