# distutils: language = c++
#cython: language_level=3

from libcpp cimport bool as cbool
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.utility cimport pair
from libcpp.map cimport map
from libcpp.unordered_map cimport unordered_map
from libcpp.memory cimport unique_ptr


cdef extern from "cbag/cbag.h" namespace "cbag" nogil:
    ctypedef unordered_map[string, cellview_info] lib_map_t
    ctypedef unordered_map[string, lib_map_t] netlist_map_t

    cdef void init_logging()

    cdef void write_netlist(const vector[cellview *]& cv_list, const vector[string]& name_list,
                            const vector[string]& inc_list, netlist_map_t& netlist_map, const char* fmt,
                            cbool flat, cbool shell, const char* fname) except +

cdef extern from "cbag/cbag.h" namespace "cbag::sch" nogil:
    ctypedef map[string, instance].iterator inst_iter_t

    cdef cppclass pin_figure:
        pass

    cdef cppclass instance:
        string lib_name
        string cell_name
        string view_name
        cbool is_primitive
        map[string, string] connections

        void clear_params() except +

        void set_int_param(const char* name, int value) except +

        void set_double_param(const char* name, double value) except +

        void set_bool_param(const char* name, cbool value) except +

        void set_string_param(const char* name, const char* value) except +

        void update_connection(const string& inst_name, const char* term,
                               const char* net) except +

        unsigned int width() const

        unsigned int height() const

    cdef cppclass cellview_info:
        pass
        
    cdef cppclass cellview:
        string lib_name
        string cell_name
        string view_name
        map[string, instance] instances
        map[string, pin_figure] in_terms
        map[string, pin_figure] out_terms
        map[string, pin_figure] io_terms

        cellview(const cellview& val) except +
        
        void clear_params() except +

        void set_int_param(const char* name, int value) except +

        void set_double_param(const char* name, double value) except +

        void set_bool_param(const char* name, cbool value) except +

        void set_string_param(const char* name, const char* value) except +

        void rename_pin(const char* old_name, const char* new_name) except +

        void add_pin(const char* new_name, unsigned int term_type) except +

        cbool remove_pin(const char* name) except +

        void rename_instance(const char* old_name, const char* new_name) except +

        cbool remove_instance(const char* name) except +

        inst_iter_t copy_instance(const char* old_name, const string& new_name,
                                  int dx, int dy, vector[pair[string, string]] conns) except +

        vector[inst_iter_t] array_instance(const char* old_name, const vector[string]& name_list,
                                           int dx, int dy,
                                           const vector[vector[pair[string, string]]]& conn_list) except +

cdef extern from "cbagyaml/cbagyaml.h" namespace "cbag" nogil:
    cdef unique_ptr[cellview] cv_from_file(const char* yaml_fname, const char* sym_view) except +

    cdef netlist_map_t read_netlist_map(const char* fname) except +


cdef class PySchCellView:
    cdef unique_ptr[cellview] cv_ptr
    cdef unicode encoding

    
cdef _add_py_sch_cv(vector[cellview *]& cv_list, PySchCellView pycv)
