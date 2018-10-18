# distutils: language = c++

from cython.operator import dereference as deref
from cython.operator import preincrement as preinc

from typing import Tuple, Iterable

from interval cimport *

# initialize logging
init_logging()


cdef class PyDisjointIntervals:
    """A data structure that keeps track of disjoint intervals.
    """
    def __init__(self):
        pass

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
        return ans

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

    def remove(self, intv):
        # type: (Tuple[int, int]) -> bool
        """Removes the given interval from this PyDisjointIntervals.

        Parameters
        ----------
        intv : Tuple[int, int]
            the interval to remove.

        Returns
        -------
        success : bool
            True if the given interval is found and removed.  False otherwise.
        """
        return self._intvs.remove(intv)

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
        return ans

    def remove_all_overlaps(self, intv):
        # type: (Tuple[int, int]) -> bool
        """Remove all intervals in this set that overlaps with the given interval.

        Parameters
        ----------
        intv : Tuple[int, int]
            the given interval

        Returns
        -------
        success : bool
            True if any interval is removed.  False otherwise.
        """
        return self._intvs.remove_overlaps(intv)
