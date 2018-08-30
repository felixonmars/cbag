# distutils: language = c++

from pybag.schematic cimport *

from cython.operator cimport dereference as deref

from libcpp.unordered_set cimport unordered_set

cdef extern from "cbagoa/cbagoa.h" namespace "cbagoa" nogil:
    cdef cppclass oa_database:
        oa_database(const char* lib_def_file) except +

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
                                const vector[cellview *]& cv_list) except +


cdef class PyOADatabase:
    cdef unique_ptr[oa_database] db_ptr
    cdef unicode encoding

    def __init__(self, lib_def_file, unicode encoding):
        lib_def_file = lib_def_file.encode(encoding)
        self.db_ptr.reset(new oa_database(lib_def_file))
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
        deref(self.db_ptr).create_lib(library, lib_path, tech_lib)

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
        cdef vector[cellview *] cv_vec
        cdef int num = len(content_list)

        cell_vec.reserve(num)
        cv_vec.reserve(num)
        for name, cv in content_list:
            cell_vec.push_back(name.encode(self.encoding))
            _add_py_cv(cv_vec, cv)

        deref(self.db_ptr).implement_sch_list(lib_name, cell_vec, sch_view, sym_view, cv_vec)
