# distutils: language = c++

from cython.operator cimport dereference as deref

from libcpp.memory cimport unique_ptr
from libcpp.string cimport string
from libcpp.utility cimport pair
from libcpp.vector cimport vector
from libcpp.unordered_map cimport unordered_map
from libcpp.unordered_set cimport unordered_set


cdef extern from "cbagoa/cbagoa.h" namespace "cbagoa":
    cdef cppclass LibDefObserver:
        pass

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


cdef class PyOADatabase:
    cdef unique_ptr[OADatabase] db_ptr
    cdef unicode encoding

    def __cinit__(self, unicode lib_def_file, unicode encoding):
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

