# distutils: language = c++
#cython: language_level=3

from libcpp cimport bool as cbool
from libcpp.utility cimport pair
from libcpp.vector cimport vector

ctypedef unsigned int uint32_t

cdef extern from "<vector>" namespace "std" nogil:
    ctypedef size_t size_type

cdef extern from "cbag/cbag.h" namespace "cbag" nogil:
    ctypedef int coord_t

    cdef void init_logging()

ctypedef void * void_ptr
ctypedef pair[coord_t, coord_t] intv_type

cdef extern from "cbag/cbag.h" namespace "cbag::util" nogil:

    cdef cppclass disjoint_intvs:
        ctypedef vector[pair[intv_type, void_ptr]].const_iterator const_iterator

        disjoint_intvs()

        const_iterator begin() const
        const_iterator end() const
        pair[const_iterator, const_iterator] overlap_range(const intv_type& key) const
        cbool empty() const
        size_type size() const
        coord_t start() except +
        coord_t stop() except +
        const_iterator find_exact(const intv_type& key) const
        cbool contains(const intv_type& key) const
        cbool overlaps(const intv_type& key) const
        cbool covers(const intv_type& key) const

        disjoint_intvs get_intersect(const disjoint_intvs& other) const
        disjoint_intvs get_complement(coord_t lower, coord_t upper) const
        disjoint_intvs get_transform(coord_t scale, coord_t shift) const

        void clear()
        void_ptr remove(const intv_type& key)
        vector[void_ptr] remove_overlaps(const intv_type& key)
        vector[void_ptr] subtract(const intv_type& key)
        cbool add(const intv_type& key, void_ptr value, cbool merge, cbool abut)

cdef class PyDisjointIntervals:
    cdef disjoint_intvs _intvs

    cdef _increment_references(self)