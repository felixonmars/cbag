# distutils: language = c++

from .util cimport rectangle, transformation

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.utility cimport pair
from libcpp.unordered_map cimport unordered_map
from libcpp cimport bool as cbool
from libcpp.memory cimport unique_ptr

ctypedef int int32_t
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

    cdef cppclass point_t:
        int32_t x() const
        int32_t y() const

    cdef cppclass pt_vector:
        pt_vector()

        void reserve(unsigned int)

        point_t& emplace_back(coord_t x, coord_t y)

        point_t& operator[](uint32_t idx)

        void pop_back()

    cdef cppclass polygon:
        pass

    cdef cppclass blockage:
        pass

    cdef cppclass boundary:
        pass

    cdef cppclass via:
        rectangle bot_box() const
        rectangle top_box() const

    cdef cppclass vector_obj_ref[T]:
        vector_obj_ref()

        T& value()

    cdef cppclass path_ref:
        path_ref()

    cdef cppclass instance:
        transformation xform

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

        vector_obj_ref[polygon] add_poly(const char* layer, const char* purpose,
                                         pt_vector data) except +

        vector_obj_ref[blockage] add_blockage(const char* layer, uint8_t blk_code,
                                              pt_vector data) except +

        vector_obj_ref[boundary] add_boundary(uint8_t bnd_code, pt_vector data) except +

        path_ref add_path(const char* layer, const char* purpose, const pt_vector& data,
                          offset_t half_width, uint8_t style0, uint8_t style1,
                          uint8_t stylem) except +

        path_ref add_path45_bus(const char* layer, const char* purpose, const pt_vector& data,
                                const vector[offset_t]& widths, const vector[offset_t]& spaces,
                                uint8_t style0, uint8_t style1, uint8_t stylem) except +

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
    cdef path_ref _ref


cdef class PyBlockage:
    cdef vector_obj_ref[blockage] _ref


cdef class PyBoundary:
    cdef vector_obj_ref[boundary] _ref


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
