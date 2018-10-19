# -*- coding: utf-8 -*-

import sys

import pytest

from pybag.util import PyDisjointIntervals


@pytest.fixture
def empty_intvs():
    """An empty interval object."""
    return PyDisjointIntervals()


@pytest.fixture
def intvs_no_val():
    """Some intervals with no values"""
    return PyDisjointIntervals(intv_list=[(1, 3), (4, 7), (7, 9), (11, 15), (16, 20)])


@pytest.fixture
def intvs_val():
    """Some intervals with values"""
    return PyDisjointIntervals(intv_list=[(1, 3), (4, 7), (7, 9), (11, 15), (16, 20)],
                               val_list=['A', 'B', 'C', 'D', 'E'])


@pytest.mark.parametrize("intvs,vals", [
    ([(1, 2), (3, 5)], ['A', 'B']),
    ([(1, 2), (4, 5), (6, 8)], [1, 2, 3]),
])
def test_constructor_refcount(intvs, vals):
    """Check Cython increment reference count of value objects."""
    before_count = [sys.getrefcount(v) for v in vals]
    # keep reference to PyDisjointInterval to avoid garbage collection
    # noinspection PyUnusedLocal
    dis_intvs = PyDisjointIntervals(intv_list=intvs, val_list=vals)
    after_count = [sys.getrefcount(v) for v in vals]
    for bc, ac in zip(before_count, after_count):
        print(bc, ac)
        assert ac == bc + 1
