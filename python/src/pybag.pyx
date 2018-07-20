# distutils: language = c++

from cython.operator cimport dereference as deref
from cython.operator cimport preincrement as inc

from libcpp.memory cimport unique_ptr
from libcpp.string cimport string
from libcpp.map cimport map
from libcpp.utility cimport pair
from libcpp.vector cimport vector
from libcpp.unordered_map cimport unordered_map
from libcpp.unordered_set cimport unordered_set

import numbers


cdef extern from "cbag/cbag.h" namespace "cbag":
    cdef void init_logging()

    cdef cppclass Instance:
        string lib_name
        string cell_name
        string view_name
        map[string, string] connections

        void clear_params() except +

        void set_int_param(const char* name, int value) except +

        void set_double_param(const char* name, double value) except +

        void set_bool_param(const char* name, bool value) except +

        void set_string_param(const char* name, const char* value) except +
        
    cdef cppclass SchCellView:
        SchCellView(const string& yaml_fname) except +

        map[string, Instance] instances


cdef extern from "cbagoa/cbagoa.h" namespace "cbagoa":
    cdef cppclass OADatabase:
        OADatabase(const char* lib_def_file) except +

        vector[string] get_cells_in_library(const char* library) except +

        string get_lib_path(const char* library) except +

        void create_lib(const char* library, const char* lib_path,
                        const char* tech_lib) except +

        vector[pair[string, string]] read_sch_recursive(const char* lib_name, const char* cell_name,
                                                        const char* view_name,
                                                        const char* new_root_path,
                                                        const unordered_map[string, string]& lib_map,
                                                        const unordered_set[string]& exclude_libs) except +

# initialize logging
init_logging()


cdef class PySchInstance:
    cdef map[string, Instance].iterator ptr
    cdef bool _static
    cdef encoding
    cdef _master
    cdef _db

    def __init__(self, db, encoding):
        self._static = False
        self.encoding = encoding
        self._master = None
        self._db = db
        

    def change_generator(self, gen_lib_name, gen_cell_name, bool static=False):
        self._master = None
        deref(self.ptr).second.lib_name = gen_lib_name
        deref(self.ptr).second.cell_name = gen_cell_name
        self._static = static
        deref(self.ptr).second.clear_params()
        deref(self.ptr).second.connections.clear()

    @property
    def name(self):
        return deref(self.ptr).first.decode(self.encoding)
    
    @property
    def is_primitive(self):
        if self._static:
            return True
        if self._master is None:
            raise ValueError('Instance {} has no master.  Did you forget to call design()?'.format(self.name))
        return self._master.is_primitive()
        
    @property
    def should_delete(self):
        return self._master is not None and self._master.should_delete_instance()

    @property
    def master(self):
        return self._master

    @property
    def gen_lib_name(self):
        return deref(self.ptr).second.lib_name.decode(self.encoding)

    @property
    def gen_cell_name(self):
        return deref(self.ptr).second.cell_name.decode(self.encoding)
    
    @property
    def master_cell_name(self):
        return self.gen_cell_name if self._master is None else self._master.cell_name

    @property
    def master_key(self):
        return self._master.key

    def get_master_lib_name(self, impl_lib):
        return self.gen_lib_name if self.is_primitive else impl_lib

    def design_specs(self, *args, **kwargs):
        self._update_master('design_specs', args, kwargs)

    def design(self, *args, **kwargs):
        self._update_master('design', args, kwargs)

    def _update_master(self, design_fun, args, kwargs):
        if args:
            key = 'args'
            idx = 1
            while key in kwargs:
                key = 'args_%d' % idx
                idx += 1
            kwargs[key] = args
        else:
            key = None
        self._master = self._db.new_master(self.gen_lib_name, self.gen_cell_name,
                                           params=kwargs, design_args=key,
                                           design_fun=design_fun)

        cdef char* ckey;
        if self._master.is_primitive():
            for key, val in self._master.get_schematic_parameters().items():
                ckey = key.encode(self.encoding)
                if isinstance(val, str):
                    deref(self.ptr).second.set_string_param(ckey, val.encode(self.encoding))
                elif isinstance(val, numbers.Integral):
                    deref(self.ptr).second.set_int_param(ckey, val)
                elif isinstance(val, numbers.Real):
                    deref(self.ptr).second.set_double_param(ckey, val)
                elif isinstance(val, bool):
                    deref(self.ptr).second.set_bool_param(ckey, val)
                else:
                    raise ValueError('Unsupported value type: {}'.format(val))

    def implement_design(self, lib_name, top_cell_name='', prefix='', suffix='', **kwargs):
        debug = kwargs.get('debug', False)
        rename_dict = kwargs.get('rename_dict', None)

        if not top_cell_name:
            top_cell_name = None

        if 'lib_path' in kwargs:
            self._db.lib_path = kwargs['lib_path']
        self._db.cell_prefix = prefix
        self._db.cell_suffix = suffix
        self._db.instantiate_masters([self._master], [top_cell_name], lib_name=lib_name,
                                     debug=debug, rename_dict=rename_dict)
    
    
cdef class PySchCellView:
    cdef unique_ptr[SchCellView] cv_ptr
    cdef unicode encoding

    def __init__(self, unicode yaml_fname, unicode encoding):
        self.cv_ptr.reset(new SchCellView(yaml_fname.encode(encoding)))
        self.encoding = encoding

    def __dealloc__(self):
        self.cv_ptr.reset()

    def close(self):
        self.cv_ptr.reset()

    
cdef class PyOADatabase:
    cdef unique_ptr[OADatabase] db_ptr
    cdef unicode encoding

    def __init__(self, unicode lib_def_file, unicode encoding):
        pylib_def_str = lib_def_file.encode(encoding)
        cdef char* clib_def_str = pylib_def_str
        self.db_ptr.reset(new OADatabase(clib_def_str))
        self.encoding = encoding

    def __dealloc__(self):
        self.db_ptr.reset()

    def close(self):
        self.db_ptr.reset()

    def get_cells_in_library(self, unicode library):
        pylib = library.encode(self.encoding)
        cdef char* clib = pylib
        cdef vector[string] ans = deref(self.db_ptr).get_cells_in_library(clib)
        return [v.decode(self.encoding) for v in ans]

    def get_lib_path(self, unicode library):
        pylib = library.encode(self.encoding)
        cdef char* clib = pylib
        cdef string ans = deref(self.db_ptr).get_lib_path(clib)
        return ans.decode(self.encoding)

    def create_lib(self, unicode library, unicode lib_path, unicode tech_lib):
        pylib = library.encode(self.encoding)
        pypath = lib_path.encode(self.encoding)
        pytech = tech_lib.encode(self.encoding)
        cdef char* clib = pylib
        cdef char* cpath = pypath
        cdef char* ctech = pytech
        deref(self.db_ptr).create_lib(clib, cpath, ctech);

    def read_sch_recursive(self, unicode lib_name, unicode cell_name, unicode view_name,
                           unicode new_root_path, lib_map, exclude_libs):
        pylib = lib_name.encode(self.encoding)
        pycell = cell_name.encode(self.encoding)
        pyview = view_name.encode(self.encoding)
        pyroot = new_root_path.encode(self.encoding)
        cdef char* clib = pylib
        cdef char* ccell = pycell
        cdef char* cview = pyview
        cdef char* croot = pyroot
        cdef unordered_set[string] exc_set
        cdef unordered_map[string, string] cmap
        for v in exclude_libs:
            exc_set.insert(v.encode(self.encoding))
        for key, val in lib_map.items():
            cmap[key.encode(self.encoding)] = val.encode(self.encoding)

        cdef vector[pair[string, string]] ans = deref(self.db_ptr).read_sch_recursive(clib, ccell, cview, croot,
                                                                                      cmap, exc_set)
        return [(p.first.decode(self.encoding), p.second.decode(self.encoding))
                for p in ans]

