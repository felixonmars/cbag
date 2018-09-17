# distutils: language = c++

from util cimport *

import pprint
from typing import Generator, List, Tuple, Optional

from .pyutil import Orientation


# initialize logging
init_logging()


cdef transformation make_transform(loc, orient):
    return transformation(loc[0], loc[1], Orientation[orient].value)


cdef class BBox:
    """An immutable bounding box.

    Parameters
    ----------
    left : float or int
        left coordinate.
    bottom : float or int
        bottom coordinate.
    right : float or int
        right coordinate.
    top : float or int
        top coordinate.
    **kwargs :
        deprecated parameters.
    """
    def __init__(self, int xl, int yl, int xh, int yh, *args, **kwargs):
        self._xl = xl
        self._yl = yl
        self._xh = xh
        self._yh = yh
        
    @classmethod
    def get_invalid_bbox(cls):
        """Returns a default invalid bounding box.

        Returns
        -------
        box : bag.layout.util.BBox
            an invalid bounding box.
        """
        return cls(0, 0, -1, -1)

    @property
    def xl(self):
        return self._xl

    @property
    def xh(self):
        return self._xh

    @property
    def yl(self):
        return self._yl

    @property
    def yh(self):
        return self._yh

    cdef coord_t _xm(self):
        return (self._xl + self._xh) // 2

    cdef coord_t _ym(self):
        return (self._yl + self._yh) // 2
    
    @property
    def xm(self):
        return self._xm()

    @property
    def ym(self):
        return self._ym()

    @property
    def w(self):
        return self._xh - self._xl

    @property
    def h(self):
        return self._yh - self._yl
    
    @property
    def left_unit(self):
        return self._xl

    @property
    def right_unit(self):
        return self._xh

    @property
    def bottom_unit(self):
        return self._yl

    @property
    def top_unit(self):
        return self._yh

    @property
    def width_unit(self):
        return self.w

    @property
    def height_unit(self):
        return self.h

    @property
    def xc_unit(self):
        return self.xm

    @property
    def yc_unit(self):
        return self.ym

    def is_physical(self):
        # type: () -> bool
        """Returns True if this bounding box has positive area.

        Returns
        -------
        is_physical : bool
            True if this bounding box has positive area.
        """
        return self._xh > self._xl and self._yh > self._yl

    def is_valid(self):
        # type: () -> bool
        """Returns True if this bounding box has nonnegative area.

        Returns
        -------
        is_valid : bool
            True if this bounding box has nonnegative area.
        """
        return self._xh >= self._xl and self._yh >= self._yl
    
    def get_points(self, unit_mode=True):
        # type: (bool) -> List[Tuple[int, int]]
        """Returns this bounding box as a list of points.

        Parameters
        ----------
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        points : List[Tuple[Union[float, int], Union[float, int]]]
            this bounding box as a list of points.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        return [(self._xl, self._yl), (self._xl, self._yh),
                (self._xh, self._yh), (self._xh, self._yl)]

    def as_bbox_array(self):
        # type: () -> BBoxArray
        """Cast this BBox as a BBoxArray."""
        return BBoxArray(self)

    def as_bbox_collection(self):
        # type: () -> BBoxCollection
        """Cast this BBox as a BBoxCollection."""
        return BBoxCollection([self])

    def merge(self, bbox):
        # type: (BBox) -> BBox
        """Returns a new bounding box that's the union of this bounding box and the given one.

        Parameters
        ----------
        bbox : BBox
            the bounding box to merge with.

        Returns
        -------
        total : BBox
            the merged bounding box.
        """
        if not bbox.is_valid():
            return self
        if not self.is_valid():
            return bbox

        return BBox(min(self._xl, bbox.xl), min(self._yl, bbox.yl),
                    max(self._xh, bbox.xh), max(self._yh, bbox.yh))

    def intersect(self, bbox):
        # type: (BBox) -> BBox
        """Returns a new bounding box that's the intersection of this bounding box and the given one.

        Parameters
        ----------
        bbox : BBox
            the bounding box to intersect with.

        Returns
        -------
        intersect : BBox
            the intersection bounding box.
        """
        return BBox(max(self._xl, bbox.xl), max(self._yl, bbox.yl),
                    min(self._xh, bbox.xh), min(self._yh, bbox.yh))

    def overlaps(self, bbox):
        # type: (BBox) -> bool
        """Returns True if this BBox overlaps the given BBox."""
        return ((max(self._xl, bbox.xl) < min(self._xh, bbox.xh)) and
                (max(self._yl, bbox.yl) < min(self._yh, bbox.yh)))

    def extend(self, x=None, y=None, unit_mode=True):
        # type: (Optional[int], Optional[int], bool) -> BBox
        """Returns an extended BBox that covers the given point.

        Parameters
        ----------
        x : Optional[int]
            if given, the X coordinate to extend to.
        y : Optional[int]
            if given, the Y coordinate to extend to
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        ext_box : BBox
            the extended bounding box.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        if x is None:
            x = self._xl
        if y is None:
            y = self._yl

        return BBox(min(self._xl, x), min(self._yl, y),
                    max(self._xh, x), max(self._yh, y))

    def expand(self, int dx=0, int dy=0, unit_mode=True):
        # type: (int, int, bool) -> BBox
        """Returns a BBox expanded by the given amount.

        Parameters
        ----------
        dx : int
            if given, expand left and right edge by this amount.
        dy : int
            if given, expand top and bottom edge by this amount.
        unit_mode : bool
            True if x and y are given in resolution units.

        Returns
        -------
        ext_box : BBox
            the extended bounding box.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        return BBox(self._xl - dx, self._yl - dy, self._xh + dx, self._yh + dy)

    cdef BBox _transform(self, const transformation& xform):
        cdef rectangle r = rectangle(self._xl, self._yl, self._xh, self._yh)
        r.transform(xform)
        return BBox(r.xl(), r.yl(), r.xh(), r.yh())

    def transform(self, loc=(0, 0), orient='R0', unit_mode=True):
        # type: (Tuple[int, int], str, bool) -> BBox
        """Returns a new BBox under the given transformation.

        rotates first before shift.

        Parameters
        ----------
        loc : Tuple[int, int]
            location of the anchor.
        orient : str
            the orientation of the bounding box.
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        box : BBox
            the new bounding box.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        return self._transform(make_transform(loc, orient))

    def move_by(self, int dx=0, int dy=0, unit_mode=True):
        # type: (int, int, bool) -> BBox
        """Returns a new BBox shifted by the given amount.

        Parameters
        ----------
        dx : int
            shift in X direction.
        dy : int
            shift in Y direction.
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        box : BBox
            the new bounding box.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        return BBox(self._xl + dx, self._yl + dy, self._xh + dx, self._yh + dy)

    def flip_xy(self):
        # type: () -> BBox
        """Returns a new BBox with X and Y coordinate swapped."""
        return BBox(self._yl, self._xl, self._yh, self._xh)

    def with_interval(self, direction, int lower, int upper, unit_mode=True):
        # type: (str, int, int, bool) -> BBox
        """Returns a BBox with the given interval replaced by the given one.

        Parameters
        ----------
        direction : str
            the given interval type, either 'x' or 'y'.
        lower : int
            interval lower bound.
        upper : int
            interval upper bound.
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        bbox : BBox
            the new bounding box.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        if direction == 'x':
            return BBox(lower, self._yl, upper, self._yh)
        else:
            return BBox(self._xl, lower, self._xh, upper)

    def get_interval(self, direction, unit_mode=True):
        # type: (str, bool) -> Tuple[int, int]
        """Returns the interval of this bounding box along the given direction.

        Parameters
        ----------
        direction : str
            direction along which to campute the bounding box interval.  Either 'x' or 'y'.
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        intv : Tuple[int, int]
            the corresponding interval of this bbox.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        if direction == 'x':
            return self._xl, self._xh
        else:
            return self._yl, self._yh

    def get_bounds(self, unit_mode=True):
        # type: (bool) -> Tuple[int, int, int, int]
        """Returns the bounds of this bounding box.

        Parameters
        ----------
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        bounds : Tuple[int, int, int, int]
            a tuple of (left, bottom, right, top) coordinates.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        return self._xl, self._yl, self._xh, self._yh

    def get_immutable_key(self):
        return self

    def __str__(self):
        return repr(self)

    def __repr__(self):
        return '{}({}, {}, {}, {})'.format(self.__class__.__name__, self._xl, self._yl, self._xh, self._yh)

    def __hash__(self):
        return hash((self._xl, self._yl, self._xh, self._yh))

    def __eq__(self, other):
        return self._xl == other.xl and self._yl == other.yl and self._xh == other.xh and self._yh == other.yh


cdef class BBoxArray:
    """An array of bounding boxes.

    Useful for representing bus of wires.

    Parameters
    ----------
    bbox : BBox
        the lower-left bounding box.
    nx : int
        number of columns.
    ny : int
        number of rows.
    spx : int
        column pitch.
    spy : int
        row pitch.
    unit_mode : bool
        deprecated parmeter.
    """

    def __init__(self, BBox bbox, int nx=1, int ny=1, int spx=0, int spy=0, unit_mode=True):
        # type: (BBox, int, int, int, int, bool) -> None
        if nx <= 0 or ny <= 0:
            raise ValueError('Cannot have 0 bounding boxes.')
        if spx < 0 or spy < 0:
            raise ValueError('Currently does not support negative pitches.')

        self._bbox = bbox
        self._nx = nx
        self._ny = ny
        self._spx = spx
        self._spy = spy

    def __iter__(self):
        # type: () -> Generator[BBox]
        """Iterates over all bounding boxes in this BBoxArray.

        traverses from left to right, then from bottom to top.
        """
        for idx in range(self._nx * self._ny):
            yield self.get_bbox(idx)

    @property
    def base(self):
        # type: () -> BBox
        """the lower-left bounding box"""
        return self._bbox

    @property
    def nx(self):
        # type: () -> int
        return self._nx

    @property
    def ny(self):
        # type: () -> int
        return self._ny

    @property
    def spx_unit(self):
        # type: () -> int
        return self._spx

    @property
    def spy_unit(self):
        # type: () -> int
        return self._spy

    @property
    def xl(self):
        return self._bbox.xl

    @property
    def xh(self):
        return self._bbox.xh

    @property
    def yl(self):
        return self._bbox.yl

    @property
    def yh(self):
        return self._bbox.yh

    @property
    def left_unit(self):
        # type: () -> int
        return self._bbox.xl

    @property
    def right_unit(self):
        # type: () -> int
        return self._bbox.xh + self._spx * (self._nx - 1)

    @property
    def bottom_unit(self):
        # type: () -> int
        return self._bbox.yl

    @property
    def top_unit(self):
        # type: () -> int
        return self._bbox.yh + self._spy * (self._ny - 1)

    @property
    def xc_unit(self):
        # type: () -> int
        return (self.left_unit + self.right_unit) // 2

    @property
    def yc_unit(self):
        # type: () -> int
        return (self.bottom_unit + self.top_unit) // 2

    def as_bbox_collection(self):
        # type: () -> BBoxCollection
        return BBoxCollection([self])

    def get_bbox(self, int idx):
        # type: (int) -> BBox
        """Returns the bounding box with the given index.

        index increases from left to right, then from bottom to top.  lower-left box is index 0.

        Returns
        -------
        bbox : BBox
            the bounding box with the given index.
        """
        row_idx, col_idx = divmod(idx, self._nx)
        return self._bbox.move_by(dx=col_idx * self._spx, dy=row_idx * self._spy)

    cpdef BBox get_overall_bbox(self):
        # type: () -> BBox
        """Returns the overall bounding box of this BBoxArray.

        Returns
        -------
        overall_bbox : BBox
            the overall bounding box of this BBoxArray.
        """
        return BBox(self._bbox._xl, self._bbox.yl,
                    self._bbox.xh + self._spx * (self._nx - 1),
                    self._bbox.yh + self._spy * (self._ny - 1))

    def move_by(self, int dx=0, int dy=0, unit_mode=True):
        # type: (int, int, bool) -> BBoxArray
        """Returns a new BBoxArray shifted by the given amount.

        Parameters
        ----------
        dx : int
            shift in X direction.
        dy : int
            shift in Y direction.
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        box_arr : BBoxArray
            the new BBoxArray.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
            
        return BBoxArray(self._bbox.move_by(dx=dx, dy=dy), nx=self._nx, ny=self._ny,
                         spx=self._spx, spy=self._spy)

    def transform(self, loc=(0, 0), orient='R0', unit_mode=True):
        # type: (Tuple[int, int], str, bool) -> BBoxArray
        """Returns a new BBoxArray under the given transformation.

        rotates first before shift.

        Parameters
        ----------
        loc : Tuple[int, int]
            location of the anchor.
        orient : str
            the orientation of the bounding box.
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        box_arr : BBoxArray
            the new BBoxArray.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')

        if orient == 'R0':
            left = self.left_unit + loc[0]
            bottom = self.bottom_unit + loc[1]
        elif orient == 'MX':
            left = self.left_unit + loc[0]
            bottom = -self.top_unit + loc[1]
        elif orient == 'MY':
            left = -self.right_unit + loc[0]
            bottom = self.bottom_unit + loc[1]
        elif orient == 'R180':
            left = -self.right_unit + loc[0]
            bottom = -self.top_unit + loc[1]
        else:
            raise ValueError('Invalid orientation: ' + orient)

        # no 90 degree-ish rotation; width and height will not interchange
        new_base = BBox(left, bottom, left + self._bbox.w, bottom + self._bbox.h)
        return BBoxArray(new_base, nx=self._nx, ny=self._ny,
                         spx=self._spx, spy=self._spy)

    def arrayed_copies(self, int nx=1, int ny=1, int spx=0, int spy=0, unit_mode=True):
        # type: (int, int, int, int, bool) -> BBoxCollection
        """Returns a BBoxCollection containing arrayed copies of this BBoxArray

        Parameters
        ----------
        nx : int
            number of copies in horizontal direction.
        ny : int
            number of copies in vertical direction.
        spx : int
            pitch in horizontal direction.
        spy : int
            pitch in vertical direction.
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        bcol : BBoxCollection
            a BBoxCollection of the arrayed copies.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')

        x_info = self._array_helper(nx, spx, self.nx, self._spx)
        y_info = self._array_helper(ny, spy, self.ny, self._spy)

        base = self.base
        barr_list = [BBoxArray(base.move_by(dx, dy), nx=new_nx, ny=new_ny,
                               spx=new_spx, spy=new_spy)
                     for new_nx, new_spx, dx in zip(*x_info)
                     for new_ny, new_spy, dy in zip(*y_info)]
        return BBoxCollection(barr_list)

    @staticmethod
    def _array_helper(int n1, int sp1, int n2, int sp2):
        if n1 == 1:
            return [n2], [sp2], [0]
        elif n2 == 1:
            return [n1], [sp1], [0]
        elif sp1 == sp2 * n2:
            return [n1 * n2], [sp2], [0]
        elif sp2 == sp1 * n1:
            return [n1 * n2], [sp1], [0]
        else:
            # no way to express as single array
            if n1 < n2 or (n1 == n2 and sp2 < sp1):
                return [n2] * n1, [sp2] * n1, list(range(0, sp1 * n1, sp1))
            else:
                return [n1] * n2, [sp1] * n2, list(range(0, sp2 * n2, sp2))

    def __str__(self):
        return repr(self)

    def __repr__(self):
        return '{}({}, nx={}, ny={}, spx={}, spy={})'.format(self.__class__.__name__, repr(self._bbox),
                                                             self._nx, self._ny, self._spx, self._spy)


cdef class BBoxCollection:
    """A collection of bounding boxes.

    To support efficient computation, this class stores bounding boxes as a list of
    BBoxArray objects.

    Parameters
    ----------
    box_arr_list : List[BBoxArray]
        list of BBoxArrays in this collections.
    """

    def __init__(self, box_arr_list):
        self._box_arr_list = box_arr_list

    def __iter__(self):
        """Iterates over all BBoxArray in this collection."""
        return self._box_arr_list.__iter__()

    def __reversed__(self):
        return self._box_arr_list.__reversed__()

    def __len__(self):
        return len(self._box_arr_list)

    def as_bbox_array(self):
        # type: () -> BBoxArray
        """Attempt to cast this BBoxCollection into a BBoxArray.

        Returns
        -------
        bbox_arr : BBoxArray
            the BBoxArray object that's equivalent to this BBoxCollection.

        Raises
        ------
        ValueError :
            if this BBoxCollection cannot be cast into a BBoxArray.
        """
        if len(self._box_arr_list) != 1:
            raise ValueError('Unable to cast this BBoxCollection into a BBoxArray.')

        return self._box_arr_list[0]

    def as_bbox(self):
        # type: () -> BBox
        """Attempt to cast this BBoxCollection into a BBox.

        Returns
        -------
        bbox : BBox
            the BBox object that's equivalent to this BBoxCollection.

        Raises
        ------
        Exception :
            if this BBoxCollection cannot be cast into a BBox.
        """
        if len(self._box_arr_list) != 1:
            raise Exception('Unable to cast this BBoxCollection into a BBoxArray.')
        box_arr = self._box_arr_list[0]
        if box_arr.nx != 1 or box_arr.ny != 1:
            raise Exception('Unable to cast this BBoxCollection into a BBoxArray.')
        return box_arr.base

    def get_bounding_box(self):
        # type: () -> BBox
        """Returns the bounding box that encloses all boxes in this collection.

        Returns
        -------
        bbox : BBox
            the bounding box of this BBoxCollection.
        """
        box = BBox.get_invalid_bbox()
        for box_arr in self._box_arr_list:
            all_box = BBox(box_arr.left, box_arr.bottom, box_arr.right, box_arr.top,
                           box_arr.base.resolution)
            box = box.merge(all_box)

        return box

    def transform(self, loc=(0, 0), orient='R0'):
        # type: (Tuple[int, int], str) -> BBoxCollection
        """Returns a new BBoxCollection under the given transformation.

        rotates first before shift.

        Parameters
        ----------
        loc : Tuple[int, int]
            location of the anchor.
        orient : str
            the orientation of the bounding box.

        Returns
        -------
        box_collection : BBoxCollection
            the new BBoxCollection.
        """
        return BBoxCollection([box_arr.transform(loc=loc, orient=orient) for box_arr in self._box_arr_list])

    def __str__(self):
        return repr(self)

    def __repr__(self):
        return pprint.pformat(self._box_arr_list)
