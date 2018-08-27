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

import os
import numbers


ctypedef map[string, SchInstance].iterator inst_iter_t

cdef extern from "cbag/cbag.h" namespace "cbag":
    cdef void init_logging()

    cdef cppclass PinFigure:
        pass

    cdef cppclass SchInstance:
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

    cdef cppclass SchCellView:
        string lib_name
        string cell_name
        string view_name
        map[string, SchInstance] instances
        map[string, PinFigure] in_terms
        map[string, PinFigure] out_terms
        map[string, PinFigure] io_terms

        SchCellView(const char* yaml_fname, const char* sym_view) except +

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

        inst_iter_t copy_instance(const char* old_name, const string& new_name,
                                  int dx, int dy, vector[pair[string, string]] conns) except +

        vector[inst_iter_t] array_instance(const char* old_name, const vector[string]& name_list,
                                           int dx, int dy,
                                           const vector[vector[pair[string, string]]]& conn_list) except +

    cdef void write_netlist(const vector[SchCellView *]& cv_list, const vector[string]& name_list,
                            const char* cell_map, const vector[string]& inc_list, const char* fmt,
                            cbool flat, cbool shell, const char* fname) except +


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

        vector[pair[string, string]] read_library(const char* lib_anme, const char* view_name,
                                                  const char* new_root_path,
                                                  const unordered_map[string, string]& lib_map,
                                                  const unordered_set[string]& exclude_libs) except +

        void implement_sch_list(const char* lib_name, const vector[string]& cell_list,
                                const char* sch_view, const char* sym_view,
                                const vector[SchCellView *]& cv_list) except +


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
    def database(self):
        return self._db

    @property
    def master(self):
        return self._master

    @master.setter
    def master(self, new_master):
        self._master = new_master

    @property
    def gen_lib_name(self):
        return self._lib_name

    @property
    def gen_cell_name(self):
        return self._cell_name

    @property
    def master_cell_name(self):
        return self.gen_cell_name if self.master is None else self.master.cell_name

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
            # update parameters
            for key, val in self._master.get_schematic_parameters().items():
                self.set_param(key, val)

    def implement_design(self, lib_name, top_cell_name='', prefix='', suffix='',
                         debug=False, rename_dict=None, lib_path='', output='schematic',
                         **kwargs):
        if not top_cell_name:
            top_cell_name = None

        if lib_path:
            self._db.lib_path = lib_path
        self._db.cell_prefix = prefix
        self._db.cell_suffix = suffix
        self._db.instantiate_masters([self._master], [top_cell_name], lib_name=lib_name,
                                     debug=debug, rename_dict=rename_dict, output=output,
                                     **kwargs)


cdef class PySchInstance(DesignInstance):
    cdef map[string, SchInstance].iterator ptr
    cdef cbool _static
    cdef encoding

    def __init__(self, db, encoding, is_static):
        DesignInstance.__init__(self, db, '', '')
        self._static = is_static
        self.encoding = encoding

    @property
    def static(self):
        return self._static

    @property
    def gen_lib_name(self):
        return deref(self.ptr).second.lib_name.decode(self.encoding)

    @property
    def gen_cell_name(self):
        return deref(self.ptr).second.cell_name.decode(self.encoding)

    @property
    def name(self):
        return deref(self.ptr).first.decode(self.encoding)

    @property
    def width(self):
        return deref(self.ptr).second.width()

    @property
    def height(self):
        return deref(self.ptr).second.height()
    
    @property
    def is_primitive(self):
        if self._static:
            return True
        if self.master is None:
            raise ValueError('Instance {} has no master.  Did you forget to call design()?'.format(self.name))
        return self.master.is_primitive()
        
    @property
    def should_delete(self):
        return self.master is not None and self.master.should_delete_instance()
    
    @property
    def master_key(self):
        return self.master.key

    def set_cinst_prim_flag(self):
        deref(self.ptr).second.is_primitive = self.is_primitive

    def _update_master(self, design_fun, args, kwargs):
        super(PySchInstance, self)._update_master(design_fun, args, kwargs)
        # update instance cell name after master is updated
        deref(self.ptr).second.cell_name = self.master_cell_name.encode(self.encoding)
        deref(self.ptr).second.is_primitive = self.is_primitive

    def change_generator(self, gen_lib_name, gen_cell_name, cbool static=False):
        self.master = None
        deref(self.ptr).second.lib_name = gen_lib_name.encode(self.encoding)
        deref(self.ptr).second.cell_name = gen_cell_name.encode(self.encoding)
        self._static = static
        deref(self.ptr).second.clear_params()
        deref(self.ptr).second.connections.clear()

    def set_param(self, key, val):
        pykey = key.encode(self.encoding)
        if isinstance(val, str):
            pyval = val.encode(self.encoding)
            deref(self.ptr).second.set_string_param(pykey, pyval)
        elif isinstance(val, numbers.Integral):
            deref(self.ptr).second.set_int_param(pykey, val)
        elif isinstance(val, numbers.Real):
            deref(self.ptr).second.set_double_param(pykey, val)
        elif isinstance(val, bool):
            deref(self.ptr).second.set_bool_param(pykey, val)
        else:
            raise ValueError('Unsupported value for key {}: {}'.format(key, val))

    def update_connection(self, term_name, net_name):
        n_term = term_name.encode(self.encoding)
        n_net = net_name.encode(self.encoding)
        deref(self.ptr).second.update_connection(deref(self.ptr).first, n_term, n_net)

    def get_master_lib_name(self, impl_lib):
        return self.gen_lib_name if self.is_primitive else impl_lib


cdef class PySchCellView:
    cdef unique_ptr[SchCellView] cv_ptr
    cdef unicode encoding

    def __init__(self, unicode yaml_fname, unicode sym_view, unicode encoding):
        py_fname = yaml_fname.encode(encoding)
        py_sym_view = sym_view.encode(encoding)
        self.cv_ptr.reset(new SchCellView(py_fname, py_sym_view))
        self.encoding = encoding

    def __dealloc__(self):
        self.cv_ptr.reset()

    @property
    def lib_name(self):
        return deref(self.cv_ptr).lib_name.decode(self.encoding)

    @property
    def cell_name(self):
        return deref(self.cv_ptr).cell_name.decode(self.encoding)

    @cell_name.setter
    def cell_name(self, new_name):
        deref(self.cv_ptr).cell_name = new_name.encode(self.encoding)

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
        cdef map[string, SchInstance].iterator biter = deref(self.cv_ptr).instances.begin()
        cdef map[string, SchInstance].iterator eiter = deref(self.cv_ptr).instances.end()
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

    def array_instance(self, inst_dict, name, name_list, term_list, int dx=0, int dy=0):
        cdef num_inst = len(name_list)
        if num_inst != len(term_list):
            raise ValueError('name_list and term_list length mismatch.')
        
        # convert python data structures to C++ data structures
        pyname = name.encode(self.encoding)
        cdef vector[string] cname_list
        cdef vector[vector[pair[string, string]]] conns_list
        cname_list.reserve(num_inst)
        conns_list.resize(num_inst)
        for idx, (nn, term) in enumerate(zip(name_list, term_list)):
            cname_list.push_back(nn.encode(self.encoding))

            conns_list[idx].resize(len(term))
            for idx2, (key, val) in enumerate(term.items()):
                conns_list[idx][idx2].first = key.encode(self.encoding)
                conns_list[idx][idx2].second = val.encode(self.encoding)

        # array instance
        cdef vector[inst_iter_t] results
        results = deref(self.cv_ptr).array_instance(pyname, cname_list, dx, dy, conns_list)
        
        # populate instance dictionary, remove original instance
        orig_inst = inst_dict.pop(name)
        db = orig_inst.database
        is_static = orig_inst.static
        for idx, nn in enumerate(name_list):
            inst = inst_dict[nn] = PySchInstance(db, self.encoding, is_static)
            inst.ptr = results[idx]
            inst.master = orig_inst.master


def implement_netlist(content_list, cell_map, inc_list, fmt, fname,
                      encoding='utf-8', flat=True, shell=False):
    cdef vector[SchCellView *] cv_list
    cdef vector[string] name_list, cinc_list

    cell_map = cell_map.encode(encoding)
    fmt = fmt.encode(encoding)
    fname = fname.encode(encoding)

    cdef int num = len(content_list)
    cv_list.reserve(num)
    name_list.reserve(num)
    for name, cv in content_list:
        name_list.push_back(name.encode(encoding))
        _add_py_cv(cv_list, cv)

    cdef int ninc = len(inc_list)
    cinc_list.reserve(ninc)
    for inc_fname in inc_list:
        if not os.path.isfile(inc_fname):
            raise ValueError('Cannot find netlist include file: {}'.format(inc_fname))
        cinc_list.push_back(inc_fname.encode(encoding))

    write_netlist(cv_list, name_list, cell_map, cinc_list, fmt, flat, shell, fname)

cdef _add_py_cv(vector[SchCellView *]& cv_list, PySchCellView pycv):
    cv_list.push_back(pycv.cv_ptr.get())

cdef class PyOADatabase:
    cdef unique_ptr[OADatabase] db_ptr
    cdef unicode encoding

    def __init__(self, lib_def_file, unicode encoding):
        lib_def_file = lib_def_file.encode(encoding)
        self.db_ptr.reset(new OADatabase(lib_def_file))
        self.encoding = encoding

    def __dealloc__(self):
        self.db_ptr.reset()

    def close(self):
        self.db_ptr.reset()

    def get_cells_in_library(self, library):
        library = library.encode(self.encoding)
        cdef vector[string] ans = deref(self.db_ptr).get_cells_in_library(library)
        return [v.decode(self.encoding) for v in ans]

    def get_lib_path(self, library):
        library = library.encode(self.encoding)
        cdef string ans = deref(self.db_ptr).get_lib_path(library)
        return ans.decode(self.encoding)

    def create_lib(self, library, lib_path, tech_lib):
        library = library.encode(self.encoding)
        lib_path = lib_path.encode(self.encoding)
        tech_lib = tech_lib.encode(self.encoding)
        deref(self.db_ptr).create_lib(library, lib_path, tech_lib);

    def read_sch_recursive(self, lib_name, cell_name, view_name,
                           new_root_path, lib_map, exclude_libs):
        lib_name = lib_name.encode(self.encoding)
        cell_name = cell_name.encode(self.encoding)
        view_name = view_name.encode(self.encoding)
        new_root_path = new_root_path.encode(self.encoding)

        cdef unordered_set[string] exc_set
        cdef unordered_map[string, string] cmap
        for v in exclude_libs:
            exc_set.insert(v.encode(self.encoding))
        for key, val in lib_map.items():
            cmap[key.encode(self.encoding)] = val.encode(self.encoding)

        cdef vector[pair[string, string]] ans = deref(self.db_ptr).read_sch_recursive(lib_name, cell_name,
                                                                                      view_name,
                                                                                      new_root_path,
                                                                                      cmap, exc_set)
        return [(p.first.decode(self.encoding), p.second.decode(self.encoding)) for p in ans]

    def read_library(self, lib_name, view_name, new_root_path, lib_map, exclude_libs):
        lib_name = lib_name.encode(self.encoding)
        view_name = view_name.encode(self.encoding)
        new_root_path = new_root_path.encode(self.encoding)

        cdef unordered_set[string] exc_set
        cdef unordered_map[string, string] cmap
        for v in exclude_libs:
            exc_set.insert(v.encode(self.encoding))
        for key, val in lib_map.items():
            cmap[key.encode(self.encoding)] = val.encode(self.encoding)

        cdef vector[pair[string, string]] ans = deref(self.db_ptr).read_library(lib_name, view_name,
                                                                                new_root_path,
                                                                                cmap, exc_set)
        return [(p.first.decode(self.encoding), p.second.decode(self.encoding)) for p in ans]

    def implement_sch_list(self, lib_name, content_list, sch_view='schematic', sym_view='symbol'):
        lib_name = lib_name.encode(self.encoding)
        sch_view = sch_view.encode(self.encoding)
        sym_view = sym_view.encode(self.encoding)

        cdef vector[string] cell_vec
        cdef vector[SchCellView *] cv_vec
        cdef int num = len(content_list)

        cell_vec.reserve(num)
        cv_vec.reserve(num)
        for name, cv in content_list:
            cell_vec.push_back(name.encode(self.encoding))
            _add_py_cv(cv_vec, cv)

        deref(self.db_ptr).implement_sch_list(lib_name, cell_vec, sch_view, sym_view, cv_vec)
