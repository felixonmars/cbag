# distutils: language = c++

from util cimport *
from .pyutil import Orientation


# initialize logging
init_logging()


cdef class CBox:

    def __init__(self, int xl, int yl, int xh, int yh, res=1.0, unit_mode=True):
        self._xl = xl
        self._yl = yl
        self._xh = xh
        self._yh = yh
        
    @classmethod
    def get_invalid_bbox(cls):
        return cls(0, 0, -1, -1)

    @property
    def xl(self):
        return self._xl

    @property
    def xh(self):
        return self._yl

    @property
    def yl(self):
        return self._xh

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
        return self._xh > self._xl and self._yh > self._yl

    def is_valid(self):
        return self._xh >= self._xl and self._yh >= self._yl
    
    def get_points(self, unit_mode=True):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        return [(self._xl, self._yl), (self._xl, self._yh),
                (self._xh, self._yh), (self._xh, self._yl)]

    def as_bbox_array(self):
        # TODO: implement this
        return None

    def as_bbox_collection(self):
        # TODO: implemment this
        return None

    def merge(self, bbox):
        if not bbox.is_valid():
            return self
        if not self.is_valid():
            return bbox

        return CBox(min(self._xl, bbox.xl), min(self._yl, bbox.yl),
                    max(self._xh, bbox.xh), max(self._yh, bbox.yh))

    def intersect(self, bbox):
        return CBox(max(self._xl, bbox.xl), max(self._yl, bbox.yl),
                    min(self._xh, bbox.xh), min(self._yh, bbox.yh))

    def overlaps(self, bbox):
        return ((max(self._xl, bbox.xl) < min(self._xh, bbox.xh)) and
                (max(self._yl, bbox.yl) < min(self._yh, bbox.yh)))

    def extend(self, x=None, y=None, unit_mode=True):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        if x is None:
            x = self._xl
        if y is None:
            y = self._yl

        return CBox(min(self._xl, x), min(self._yl, y),
                    max(self._xh, x), max(self._yh, y))

    def expand(self, int dx=0, int dy=0, unit_mode=True):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        return CBox(self._xl - dx, self._yl - dy, self._xh + dx, self._yh + dy)

    cdef CBox _transform(self, coord_t dx, coord_t dy, uint32_t code):
        cdef transformation xform = transformation(dx, dy, code)
        cdef rectangle r = rectangle(self._xl, self._yl, self._xh, self._yh)
        r.transform(xform)
        return CBox(r.xl(), r.yl(), r.xh(), r.yh())

    def transform(self, loc=(0, 0), orient='R0', unit_mode=True):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        return self._transform(loc[0], loc[1], Orientation[orient].value)

    def move_by(self, int dx=0, int dy=0, unit_mode=True):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        return CBox(self._xl + dx, self._yl + dy, self._xh + dx, self._yh + dy)

    def flip_xy(self):
        return CBox(self._yl, self._xl, self._yh, self._xh)

    def with_interval(self, direction, int lower, int upper, unit_mode=True):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        if direction == 'x':
            return CBox(lower, self._yl, upper, self._yh)
        else:
            return CBox(self._xl, lower, self._xh, upper)

    def get_interval(self, direction, unit_mode=True):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        if direction == 'x':
            return self._xl, self._xh
        else:
            return self._yl, self._yh

    def get_bounds(self, unit_mode=True):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        return self._xl, self._yl, self._xh, self._yh

    def get_immutable_key(self):
        return self

    def __str__(self):
        return repr(self)

    def __repr__(self):
        return '{}({} {} {} {})'.format(self.__class__.__name__, self._xl, self._yl, self._xh, self._yh)

    def __hash__(self):
        return hash((self._xl, self._yl, self._xh, self._yh))

    def __eq__(self, other):
        return self._xl == other.xl and self._yl == other.yl and self._xh == other.xh and self._yh == other.yh
