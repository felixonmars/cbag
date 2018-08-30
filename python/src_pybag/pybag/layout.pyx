# distutils: language = c++

from layout cimport *


# initialize logging
init_logging()


cdef class CBox:
    def __cinit__(self):
        self._xl = 0
        self._yl = 0
        self._xh = 0
        self._yh = 0

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
        return self._xh

    @property
    def yl(self):
        return self._yl

    @property
    def yh(self):
        return self._yh
    
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
        return self._xh - self._xl

    @property
    def height_unit(self):
        return self._yh - self._yl

    @property
    def xc_unit(self):
        return (self._xl + self._xh) // 2

    @property
    def yc_unit(self):
        return (self._yl + self._yh) // 2

    def is_physical(self):
        return self._xh > self._xl and self._yh > self._yl

    def is_valid(self):
        return self._xh >= self._xl and self._yh >= self._yl
    
    def get_points(self, unit_mode=True):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        return [(self._xl, self._yl), (self._xl, self._yh),
                (self._xr, self._yh), (self._xr, self._yl)]

    def as_bbox_array(self):
        return BBoxArray(self)

    def as_bbox_collection(self):
        return BBoxCollection([BBoxArray(self)])

    def merge(self, bbox):
        if not self.is_valid():
            return bbox
        elif not bbox.is_valid():
            return self

        return BBox(min(self._xl, bbox._xl), min(self._yl, bbox._yl),
                    max(self._xr, bbox._xr), max(self._yh, bbox._yh))

    def intersect(self, bbox):
        return BBox(max(self._xl, bbox._xl), max(self._yl, bbox._yl),
                    min(self._xr, bbox._xr), min(self._yh, bbox._yh))

    def overlaps(self, bbox):
        return ((max(self._xl, bbox._xl) < min(self._xr, bbox._xr)) and
                (max(self._yl, bbox._yl) < min(self._yh, bbox._yh)))

    def extend(self, x=None, y=None, unit_mode=True):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        if x is None:
            x = self._xl
        if y is None:
            y = self._yl

        return BBox(min(self._xl, x), min(self._yl, y),
                    max(self._xr, x), max(self._yh, y))

    def expand(self, dx=0, dy=0, unit_mode=True):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')

        return BBox(self._xl - dx, self._yl - dy,
                    self._xr + dx, self._yh + dy)

    def transform(self, loc=(0, 0), orient='R0', unit_mode=False):
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')

        p1 = transform_point(self._xl, self._yl, loc, orient)
        p2 = transform_point(self._xr, self._yh, loc, orient)
        return BBox(min(p1[0], p2[0]), min(p1[1], p2[1]),
                    max(p1[0], p2[0]), max(p1[1], p2[1]),
                    self._res, unit_mode=True)

    def move_by(self, dx=0, dy=0, unit_mode=False):
        if not unit_mode:
            dx = int(round(dx / self._res))
            dy = int(round(dy / self._res))
        return BBox(self._xl + dx, self._yl + dy,
                    self._xr + dx, self._yh + dy,
                    self._res, unit_mode=True)

    def flip_xy(self):
        return BBox(self._yl, self._xl, self._yh, self._xr,
                    self._res, unit_mode=True)

    def with_interval(self, direction, lower, upper, unit_mode=False):
        if not unit_mode:
            lower = int(round(lower / self._res))
            upper = int(round(upper / self._res))
        if direction == 'x':
            return BBox(lower, self._yl, upper, self._yh, self._res, unit_mode=True)
        else:
            return BBox(self._xl, lower, self._xr, upper, self._res, unit_mode=True)

    def get_interval(self, direction, unit_mode=False):
        if direction == 'x':
            ans = self._xl, self._xr
        else:
            ans = self._yl, self._yh

        if unit_mode:
            return ans
        return ans[0] * self._res, ans[1] * self._res

    def get_bounds(self, unit_mode=False):
        if unit_mode:
            return self._xl, self._yl, self._xr, self._yh
        else:
            return self.left, self.bottom, self.right, self.top

    def get_immutable_key(self):
        """Returns an immutable key object that can be used to uniquely identify this BBox."""
        return (self.__class__.__name__, self._xl, self._yl,
                self._xr, self._yh, self._res)

    def __str__(self):
        return repr(self)

    def __repr__(self):
        precision = max(1, -1 * int(np.floor(np.log10(self._res))))
        fmt_str = '%s(%.{0}f, %.{0}f, %.{0}f, %.{0}f)'.format(precision)
        return fmt_str % (self.__class__.__name__, self.left, self.bottom, self.right, self.top)

    def __hash__(self):
        return hash(self.get_immutable_key())

    def __eq__(self, other):
        return self.get_immutable_key() == other.get_immutable_key()
