# distutils: language = c++

from cython.operator cimport dereference as deref

from libcpp.memory cimport unique_ptr
from libcpp.string cimport string
from libcpp.unordered_set cimport unordered_set


cdef extern from "cbagoa/database.h" namespace "cbagoa":
    cdef cppclass LibDefObserver:
        pass

    cdef cppclass OADatabase:
        OADatabase(const char* lib_def_file) except +
        void create_lib(const char* library, const char* lib_path,
                        const char* tech_lib) except +
        void read_sch_recursive(const char* lib_name, const char* cell_name,
                                const char* view_name, const char* root_path,
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

    def create_lib(self, unicode library, unicode lib_path, unicode tech_lib):
        pylib = library.encode(self.encoding)
        pypath = lib_path.encode(self.encoding)
        pytech = tech_lib.encode(self.encoding)
        cdef char* clib = pylib
        cdef char* cpath = pypath
        cdef char* ctech = pytech
        deref(self.db_ptr).create_lib(clib, cpath, ctech);

    def read_sch_recursive(self, unicode lib_name, unicode cell_name, unicode view_name,
                           unicode root_path, exclude_libs):
        pylib = lib_name.encode(self.encoding)
        pycell = cell_name.encode(self.encoding)
        pyview = view_name.encode(self.encoding)
        pyroot = root_path.encode(self.encoding)
        cdef char* clib = pylib
        cdef char* ccell = pycell
        cdef char* cview = pyview
        cdef char* croot = pyroot
        cdef unordered_set[string] exc_set = {a.encode(self.encoding) for a in exclude_libs}
        for v in exc_set:
            print(v)
        deref(self.db_ptr).read_sch_recursive(clib, ccell, cview, croot, exc_set)
