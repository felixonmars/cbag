# distutils: language = c++

from .util cimport rectangle, transformation, BBox

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp cimport bool as cbool
from libcpp.memory cimport unique_ptr
from libcpp.utility cimport pair

ctypedef int int32_t
ctypedef unsigned int uint8_t
ctypedef unsigned int uint32_t


cdef extern from "cbag/cbag.h" namespace "cbag" nogil:
    ctypedef unsigned int lay_t
    ctypedef unsigned int purp_t
    ctypedef int coord_t
    ctypedef int offset_t
    ctypedef int dist_t

    ctypedef pair[lay_t, purp_t] layer_t

    cdef void init_logging()


cdef extern from "cbag/cbag.h" namespace "cbag::layout" nogil:
    cdef cppclass tech:
        tech(const char* fname) except +

        string pin_purpose_name

        offset_t get_min_space(const string& layer_type, offset_t width, uint8_t sp_type) except +

    cdef cppclass point_t:
        point_t(coord_t x, coord_t y)

        coord_t x() const
        coord_t y() const

    cdef cppclass polygon90:
        pass

    cdef cppclass polygon45:
        pass

    cdef cppclass polygon:
        pass

    cdef cppclass polygon45_set:
        pass

    cdef cppclass blockage:
        pass

    cdef cppclass boundary:
        pass

    cdef cppclass via:
        rectangle bot_box() const
        rectangle top_box() const

    cdef cppclass instance:
        string name
        transformation xform
        uint32_t nx
        uint32_t ny
        offset_t spx
        offset_t spy

        void set_master(const cellview* new_master)

        void set_int_param(const char* name, int value)

        void set_double_param(const char* name, double value)

        void set_bool_param(const char* name, cbool value)

        void set_string_param(const char* name, const char* value)

    cdef cppclass shape_ref[T]:
        cellview* parent
        T obj

        shape_ref()
        shape_ref(const layer_t& key, T* ptr)

        void commit()

    cdef cppclass cv_obj_ref[T]:
        cellview* parent
        T obj

        cv_obj_ref()
        void commit()

    enum geo_union_enum:
        RECT
        POLY90
        POLY45
        POLY

    cdef cppclass geo_union:
        T* get_if[T]()

        geo_union_enum index() const

    cdef cppclass geo_iterator:
        cbool has_next() const

        geo_iterator &operator++()
        const geo_union &operator*() const

    cdef cppclass cellview:
        string cell_name

        cellview(tech* tech_ptr, const char* cell_name, uint8_t geo_mode)

        void set_geometry_mode(uint8_t new_mode) except +

        cbool empty() const

        layer_t get_lay_purp_key(const char* layer, const char* purpose) except +

        rectangle get_bbox(const char* layer, const char* purpose) except +

        geo_iterator begin_intersect(const layer_t& key, const rectangle& r,
                                     offset_t spx, offset_t spy) const

        void add_pin(const char *layer, coord_t xl, coord_t yl, coord_t xh, coord_t yh,
                     const char *net, const char *label) except +

        shape_ref[rectangle] add_rect(const char* layer, const char* purpose, cbool is_horiz, coord_t xl,
                                      coord_t yl, coord_t xh, coord_t yh, cbool commit) except +

        shape_ref[polygon] add_poly(const char* layer, const char* purpose, cbool is_horiz,
                                    const vector[point_t]& data, cbool commit) except +

        shape_ref[polygon45_set] add_path(const char* layer, const char* purpose, cbool is_horiz,
                                          const vector[point_t]& data,
                                          offset_t half_width, uint8_t style0, uint8_t style1,
                                          uint8_t stylem, cbool commit) except +

        shape_ref[polygon45_set] add_path45_bus(const char* layer, const char* purpose, cbool is_horiz,
                                                const vector[point_t]& data,
                                                const vector[offset_t]& widths, const vector[offset_t]& spaces,
                                                uint8_t style0, uint8_t style1, uint8_t stylem,
                                                cbool commit) except +

        cv_obj_ref[blockage] add_blockage(const char* layer, uint8_t blk_code,
                                          const vector[point_t]& data, cbool commit) except +

        cv_obj_ref[boundary] add_boundary(uint8_t bnd_code, const vector[point_t]& data, cbool commit) except +

        cv_obj_ref[via] add_via(transformation xform, const char* vid, const uint32_t (&num)[2],
                                const dist_t (&cut_dim)[2], const offset_t (&cut_sp)[2],
                                const offset_t (&lay1_enc)[2], const offset_t (&lay1_off)[2],
                                const offset_t (&lay2_enc)[2], const offset_t (&lay2_off)[2],
                                cbool add_layers, cbool bot_horiz, cbool top_horiz, cbool commit) except +

        cv_obj_ref[instance] add_prim_instance(const char* lib, const char* cell, const char* view,
                                               const char* name, transformation xform, uint32_t nx,
                                               uint32_t ny, offset_t spx, offset_t spy, cbool commit)

        cv_obj_ref[instance] add_instance(const cellview* cv, const char* name, transformation xform,
                                          uint32_t nx, uint32_t ny, offset_t spx, offset_t spy,
                                          cbool commit)


cdef class PyTech:
    cdef unique_ptr[tech] _ptr
    cdef unicode _encoding
    cdef unicode _pin_purpose


cdef class PyRect:
    cdef shape_ref[rectangle] _ref

cdef class PyPolygon90:
    cdef shape_ref[polygon90] _ref

cdef class PyPolygon45:
    cdef shape_ref[polygon45] _ref

cdef class PyPolygon:
    cdef shape_ref[polygon] _ref


cdef class PyVia:
    cdef cv_obj_ref[via] _ref


cdef class PyPath:
    cdef shape_ref[polygon45_set] _ref


cdef class PyBlockage:
    cdef cv_obj_ref[blockage] _ref


cdef class PyBoundary:
    cdef cv_obj_ref[boundary] _ref


cdef class PyLayInstance:
    cdef cv_obj_ref[instance] _ref
    cdef _grid
    cdef _master
    cdef unicode _lib_name

    cdef _update_inst_master(self, PyLayCellView cv)
    cdef _translate_master_box_w_array(self, BBox box)

cdef class PyLayCellView:
    cdef unique_ptr[cellview] _ptr
    cdef unicode _encoding

    cdef PyLayInstance _add_cinst(self, grid, master, lib_name, PyLayCellView cv,
                                  const char *inst_name, transformation xform, int nx, int ny,
                                  int spx, int spy, cbool commit)
