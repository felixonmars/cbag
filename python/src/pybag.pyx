# distutils: language = c++

from cython.operator cimport dereference as deref

from libcpp.memory cimport unique_ptr
from libcpp.string cimport string
from libcpp.map cimport map


cdef extern from "cbag/cbag.h" namespace "cbag":
    cdef cppclass Instance:
        pass

    cdef cppclass SchCellView:
        SchCellView(const char* yaml_fname) except +

        map[string, Instance] instances
        


cdef class PySchCellView:
    cdef unique_ptr[SchCellView] cv_ptr
    cdef unicode encoding

    def __cinit__(self, unicode yaml_fname, unicode encoding):
        pyfname = yaml_fname.encode(encoding)
        cdef char* cfname = pyfname
        self.cv_ptr.reset(new SchCellView(cfname))
        self.encoding = encoding

    def __dealloc__(self):
        self.cv_ptr.reset()

    def close(self):
        self.cv_ptr.reset()
