# distutils: language = c++
#cython: language_level=3

from cpython.ref cimport PyObject, Py_XINCREF, Py_XDECREF

from cython.operator import dereference as deref
from cython.operator import preincrement as preinc

from typing import Tuple, Iterable, Any

from .interval cimport *

# initialize logging
init_logging()


cdef _decrement_references(const vector[void_ptr] &vec):
    cdef n = vec.size()
    for i in range(n):
        Py_XDECREF(<PyObject*>vec[i])


cdef class PyDisjointIntervals:
    """A data structure that keeps track of disjoint intervals.
    """
    def __init__(self):
        pass

    def __dealloc__(self):
        # decrement all Python pointer counters
        cdef disjoint_intvs.const_iterator beg_iter = self._intvs.begin()
        cdef disjoint_intvs.const_iterator end_iter = self._intvs.end()
        while beg_iter != end_iter:
            Py_XDECREF(<PyObject*>deref(beg_iter).second)
            preinc(beg_iter)

        # clear C++ object content
        self._intvs.clear()

    def __contains__(self, key):
        # type: (Tuple[int, int]) -> bool
        """Returns True if this PyDisjointIntervals contains the given interval.

        Parameters
        ----------
        key : Tuple[int, int]
            the interval to test.

        Returns
        -------
        contains : bool
            True if this PyDisjointIntervals contains the given interval.
        """
        return self._intvs.contains(key)

    def __iter__(self):
        # type: () -> Iterable[Tuple[int, int]]
        """Iterates over intervals in this PyDisjointIntervals in increasing order.

        Yields
        ------
        intv : Tuple[int, int]
            the next interval.
        """
        cdef disjoint_intvs.const_iterator beg_iter = self._intvs.begin()
        cdef disjoint_intvs.const_iterator end_iter = self._intvs.end()

        cdef int a = 0
        cdef int b = 0
        while beg_iter != end_iter:
            yield deref(beg_iter).first.first, deref(beg_iter).first.second
            preinc(beg_iter)

    def __len__(self):
        # type: () -> int
        """Returns the number of intervals in this PyDisjointIntervals.

        Returns
        -------
        length : int
            number of intervals in this set.
        """
        return self._intvs.size()

    def get_start(self):
        # type: () -> int
        """Returns the start of the first interval.

        Returns
        -------
        start : int
            the start of the first interval.
        """
        return self._intvs.start()

    def get_end(self):
        # type: () -> int
        """Returns the end of the last interval.

        Returns
        -------
        end : int
            the end of the last interval.
        """
        return self._intvs.stop()

    def has_overlap(self, intv):
        # type: (Tuple[int, int]) -> bool
        """Returns True if the given interval overlaps at least one interval in this set.

        Parameters
        ----------
        intv : Tuple[int, int]
            the given interval.

        Returns
        -------
        has_overlap : bool
            True if there is at least one interval in this set that overlaps with the given one.
        """
        return self._intvs.overlaps(intv)

    def has_single_cover(self, intv):
        # type: (Tuple[int, int]) -> bool
        """Returns True if the given interval is completed covered by a single interval."""
        return self._intvs.covers(intv)

    cdef _increment_references(self):
        cdef disjoint_intvs.const_iterator beg_iter = self._intvs.begin()
        cdef disjoint_intvs.const_iterator end_iter = self._intvs.end()
        while beg_iter != end_iter:
            Py_XINCREF(<PyObject*>deref(beg_iter).second)
            preinc(beg_iter)

    def copy(self):
        # type: () -> PyDisjointIntervals
        """Create a copy of this interval set.

        Returns
        -------
        intv_set : PyDisjointIntervals
            a copy of this PyDisjointIntervals.
        """
        cdef PyDisjointIntervals ans = PyDisjointIntervals()
        ans._intvs = self._intvs

        # increment reference counts
        ans._increment_references()
        return ans

    def get_intersection(self, PyDisjointIntervals other):
        # type: (PyDisjointIntervals) -> PyDisjointIntervals
        """Returns the intersection of two PyDisjointIntervalss.

        the new PyDisjointIntervals will have all values set to None.

        Parameters
        ----------
        other : PyDisjointIntervals
            the other PyDisjointIntervals.

        Returns
        -------
        intersection : PyDisjointIntervals
            a new PyDisjointIntervals containing all intervals present in both sets.
        """
        cdef PyDisjointIntervals ans = PyDisjointIntervals()
        ans._intvs = self._intvs.get_intersect(other._intvs)
        ans._increment_references()
        return ans

    def get_complement(self, total_intv):
        # type: (Tuple[int, int]) -> PyDisjointIntervals
        """Returns a new PyDisjointIntervals that's the complement of this one.

        The new PyDisjointIntervals will have all values set to None.

        Parameters
        ----------
        total_intv : Tuple[int, int]
            the universal interval.  All intervals in this PyDisjointIntervals must be as
            subinterval of the universal interval.

        Returns
        -------
        complement : PyDisjointIntervals
            the complement of this PyDisjointIntervals.
        """
        cdef PyDisjointIntervals ans = PyDisjointIntervals()
        ans._intvs = self._intvs.get_complement(total_intv[0], total_intv[1])
        ans._increment_references()
        return ans

    def get_transform(self, int scale=1, int shift=0):
        # type: (int, int) -> PyDisjointIntervals
        """Return a new PyDisjointIntervals under the given transformation.

        Parameters
        ----------
        scale : int
            multiple all interval values by this scale.  Either 1 or -1.
        shift : int
            add this amount to all intervals.

        Returns
        -------
        intv_set : PyDisjointIntervals
            the transformed PyDisjointIntervals.
        """
        cdef PyDisjointIntervals ans = PyDisjointIntervals()
        ans._intvs = self._intvs.get_transform(scale, shift)
        ans._increment_references()
        return ans

    def remove(self, intv):
        # type: (Tuple[int, int]) -> None
        """Removes the given interval from this PyDisjointIntervals.

        Parameters
        ----------
        intv : Tuple[int, int]
            the interval to remove.
        """
        cdef void_ptr ptr = self._intvs.remove(intv)
        Py_XDECREF(<PyObject*>ptr)

    def remove_all_overlaps(self, intv):
        # type: (Tuple[int, int]) -> None
        """Remove all intervals in this set that overlaps with the given interval.

        Parameters
        ----------
        intv : Tuple[int, int]
            the given interval
        """
        cdef vector[void_ptr] values = self._intvs.remove_overlaps(intv)
        _decrement_references(values)

    def subtract(self, intv):
        # type: (Tuple[int, int]) -> None
        """Subtract the given interval from this PyDisjointIntervals.

        Parameters
        ----------
        intv : Tuple[int, int]
            the interval to subtract.
        """
        cdef vector[void_ptr] values = self._intvs.subtract(intv)
        _decrement_references(values)

    def add(self, intv, val=None, cbool merge=False, cbool abut=False):
        # type: (Tuple[int, int], Any, bool, bool) -> bool
        """Adds the given interval to this PyDisjointIntervals.

        Can only add interval that does not overlap with any existing ones, unless merge is True.

        Parameters
        ----------
        intv : Tuple[int, int]
            the interval to add.
        val : Any
            the value associated with the given interval.
        merge : bool
            If true, the given interval will be merged with any existing intervals
            that overlaps with it.  The merged interval will have the given value.
        abut : bool
            True to count merge abutting intervals.

        Returns
        -------
        success : bool
            True if the given interval is added.
        """
        cdef PyObject* py_ptr = NULL
        cdef void_ptr ptr = NULL
        if val is not None:
            py_ptr = <PyObject*>val
            ptr = <void_ptr>val

        cdef cbool success = self._intvs.add(intv, ptr, merge, abut)
        if success:
            # increment reference count
            Py_XINCREF(py_ptr)

        return success
