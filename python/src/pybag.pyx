# distutils: language = c++

from cython.operator cimport dereference as deref
from cython.operator cimport preincrement as inc

from libcpp cimport bool as cbool
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
    
    cdef cppclass PinFigure:
        pass
    
    cdef cppclass Instance:
        string lib_name
        string cell_name
        string view_name
        map[string, string] connections

        void clear_params() except +

        void set_int_param(const char* name, int value) except +

        void set_double_param(const char* name, double value) except +

        void set_bool_param(const char* name, cbool value) except +

        void set_string_param(const char* name, const char* value) except +

        void update_connection(const string& inst_name, const char* term,
                               const char* net) except +
        
    cdef cppclass SchCellView:
        string lib_name
        string cell_name
        string view_name
        map[string, Instance] instances
        map[string, PinFigure] in_terms
        map[string, PinFigure] out_terms
        map[string, PinFigure] io_terms

        SchCellView(const string& yaml_fname) except +

        void clear_params() except +

        void set_int_param(const char* name, int value) except +

        void set_double_param(const char* name, double value) except +

        void set_bool_param(const char* name, cbool value) except +

        void set_string_param(const char* name, const char* value) except +

        void rename_pin(const char* old_name, const char* new_name) except +

        void add_pin(const char* new_name, unsigned int term_type) except +

        cbool remove_pin(const char* name);

        void rename_instance(const char* old_name, const char* new_name) except +

        cbool remove_instance(const char* name) except +
        

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


cdef class DesignInstance:
    cdef _master
    cdef _db
    cdef _lib_name
    cdef _cell_name
    def __init__(self, db, lib_name, cell_name):
        self._db = db
        self._master = None
        self._lib_name = lib_name
        self._cell_name = cell_name

    @property
    def master(self):
        return self._master

    @property
    def gen_lib_name(self):
        return self._lib_name

    @property
    def gen_cell_name(self):
        return self._cell_name

    def set_param(self, key, val):
        raise Exception('Cannot set parameters on a DesignInstance; '
                        'instance of primitive maters are not allowed.')
    
    def design_specs(self, *args, **kwargs):
        self._update_master('design_specs', args, kwargs)

    def design(self, *args, **kwargs):
        self._update_master('design', args, kwargs)

    def _update_master(self, design_fun, args, kwargs):
        if args:
            key = 'args'
            idx = 1
            while key in kwargs:
                key = 'args_{:d}'.format(idx)
                idx += 1
            kwargs[key] = args
        else:
            key = None
        self._master = self._db.new_master(self.gen_lib_name, self.gen_cell_name,
                                           params=kwargs, design_args=key,
                                           design_fun=design_fun)

        if self._master.is_primitive():
            for key, val in self._master.get_schematic_parameters().items():
                self.set_param(key, val)

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


cdef class PySchInstance(DesignInstance):
    cdef map[string, Instance].iterator ptr
    cdef cbool _static
    cdef encoding

    def __init__(self, db, encoding, is_static):
        DesignInstance.__init__(self, db, '', '')
        self._static = is_static
        self.encoding = encoding

    def set_param(self, key, val):
        if isinstance(val, str):
            deref(self.ptr).second.set_string_param(key.encode(self.encoding), val.encode(self.encoding))
        elif isinstance(val, numbers.Integral):
            deref(self.ptr).second.set_int_param(key.encode(self.encoding), val)
        elif isinstance(val, numbers.Real):
            deref(self.ptr).second.set_double_param(key.encode(self.encoding), val)
        elif isinstance(val, bool):
            deref(self.ptr).second.set_bool_param(key.encode(self.encoding), val)
        else:
            raise ValueError('Unsupported value for key {}: {}'.format(key, val))

    @property
    def gen_lib_name(self):
        return deref(self.ptr).second.lib_name.decode(self.encoding)

    @property
    def gen_cell_name(self):
        return deref(self.ptr).second.cell_name.decode(self.encoding)

    def change_generator(self, gen_lib_name, gen_cell_name, cbool static=False):
        self._master = None
        deref(self.ptr).second.lib_name = gen_lib_name.encode(self.encoding)
        deref(self.ptr).second.cell_name = gen_cell_name.encode(self.encoding)
        self._static = static
        deref(self.ptr).second.clear_params()
        deref(self.ptr).second.connections.clear()

    def update_connection(self, term_name, net_name):
        n_term = term_name.encode(self.encoding)
        n_net = net_name.encode(self.encoding)
        deref(self.ptr).second.update_connection(deref(self.ptr).first, n_term, n_net)
        
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
    def master_cell_name(self):
        return self.gen_cell_name if self._master is None else self._master.cell_name

    @property
    def master_key(self):
        return self._master.key

    def get_master_lib_name(self, impl_lib):
        return self.gen_lib_name if self.is_primitive else impl_lib
    
    
cdef class PySchCellView:
    cdef unique_ptr[SchCellView] cv_ptr
    cdef unicode encoding

    def __init__(self, unicode yaml_fname, unicode encoding):
        self.cv_ptr.reset(new SchCellView(yaml_fname.encode(encoding)))
        self.encoding = encoding

    def __dealloc__(self):
        self.cv_ptr.reset()

    @property
    def lib_name(self):
        return deref(self.cv_ptr).lib_name.decode(self.encoding)

    @property
    def cell_name(self):
        return deref(self.cv_ptr).cell_name.decode(self.encoding)

    @property
    def view_name(self):
        return deref(self.cv_ptr).view_name.decode(self.encoding)

    def close(self):
        self.cv_ptr.reset()

    def clear_params(self):
        deref(self.cv_ptr).clear_params();
        
    def set_param(self, key, val):
        if isinstance(val, str):
            deref(self.cv_ptr).set_string_param(key.encode(self.encoding), val.encode(self.encoding))
        elif isinstance(val, numbers.Integral):
            deref(self.cv_ptr).set_int_param(key.encode(self.encoding), val)
        elif isinstance(val, numbers.Real):
            deref(self.cv_ptr).set_double_param(key.encode(self.encoding), val)
        elif isinstance(val, bool):
            deref(self.cv_ptr).set_bool_param(key.encode(self.encoding), val)
        else:
            raise ValueError('Unsupported value for key {}: {}'.format(key, val))

    def get_instances(self, db):
        result = {}
        cdef map[string, Instance].iterator biter = deref(self.cv_ptr).instances.begin()
        cdef map[string, Instance].iterator eiter = deref(self.cv_ptr).instances.end()
        while biter != eiter:
            key = deref(biter).first.decode(self.encoding)
            inst = PySchInstance(db, self.encoding,
                                 db.is_lib_excluded(deref(biter).second.lib_name.decode(self.encoding)))
            # invoke copy constructor
            inst.ptr = biter
            # incrementing biter is okay, because inst.ptr copied biter
            inc(biter)
            result[key] = inst

        return result

    def get_inputs(self):
        return {pair.first.decode(self.encoding) for pair in deref(self.cv_ptr).in_terms}

    def get_outputs(self):
        return {pair.first.decode(self.encoding) for pair in deref(self.cv_ptr).out_terms}

    def get_inouts(self):
        return {pair.first.decode(self.encoding) for pair in deref(self.cv_ptr).io_terms}

    def rename_pin(self, old_name, new_name):
        oldn = old_name.encode(self.encoding)
        newn = new_name.encode(self.encoding)
        deref(self.cv_ptr).rename_pin(oldn, newn)

    def add_pin(self, new_name, pin_type):
        if pin_type == 'input':
            term_type = 0
        elif pin_type == 'output':
            term_type = 1
        elif pin_type == 'inputOutput':
            term_type = 2
        else:
            raise ValueError('Unknown pin type: {}'.format(pin_type))

        newn = new_name.encode(self.encoding)
        deref(self.cv_ptr).add_pin(newn, term_type)

    def remove_pin(self, name):
        n = name.encode(self.encoding)
        return deref(self.cv_ptr).remove_pin(n)
        
    def rename_instance(self, old_name, new_name):
        oldn = old_name.encode(self.encoding)
        newn = new_name.encode(self.encoding)
        deref(self.cv_ptr).rename_instance(oldn, newn)

    def remove_instance(self, name):
        n = name.encode(self.encoding)
        return deref(self.cv_ptr).remove_instance(n)

    
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

