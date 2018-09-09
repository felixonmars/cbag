# distutils: language = c++

from cellview cimport *

from cython.operator cimport dereference as deref

import numbers

cdef extern from "<utility>" namespace "std" nogil:
    cdef unique_ptr[tech] move(unique_ptr[tech])


# initialize logging
init_logging()


cdef class PyTech:
    def __init__(self, layer_file, encoding):
        layer_file = layer_file.encode(encoding)
        self._ptr = move(tech_from_file(layer_file))

    def __dealloc__(self):
        self._ptr.reset()


cdef class PyRect:
    def __init__(self):
        pass


cdef class PyVia:
    def __init__(self):
        pass


cdef class PyPath:
    def __init__(self):
        pass


cdef class PyBlockage:
    def __init__(self):
        pass


cdef class PyBoundary:
    def __init__(self):
        pass


cdef class PyLayInstance:
    def __init__(self, grid, master, lib_name):
        self._grid = grid
        self._master = master
        self._lib_name = lib_name


cdef class PyLayCellView:
    def __init__(self, PyTech tech, encoding, int geo_mode=0):
        self._encoding = encoding
        self._ptr.reset(new cellview(tech._ptr.get(), geo_mode))

    def __dealloc__(self):
        self._ptr.reset()

    @property
    def is_empty(self):
        # type: () -> bool
        return deref(self._ptr).empty()

    def finalize(self):
        pass
    
    def get_rect_bbox(self, layer):
        # type: (Union[str, Tuple[str, str]]) -> BBox
        """Returns the overall bounding box of all rectangles on the given layer.

        Note: currently this does not check primitive instances or vias.

        Parameters
        ----------
        layer : Union[str, Tuple[str, str]]
            the layer name or layer/purpose pair.

        Returns
        -------
        bbox : BBox
            the bounding box.
        """
        if isinstance(layer, str):
            purpose = None
            layer = layer[0].encode(self._encoding)
        else:
            purpose = layer[1].encode(self._encoding)
            layer = layer[0].encode(self._encoding)

        cdef rectangle r = deref(self._ptr).get_bbox(layer, purpose)
        return BBox(r.xl(), r.yl(), r.xh(), r.yh())
    
    def get_masters_set(self):
        """Returns a set of all template master keys used in this layout."""
        # TODO: implement this
        return []

    def add_prim_instance(self, lib, cell, view, inst_name, params, int dx, int dy, int ocode,
                     int nx, int ny, int spx, int spy):
        # type: (str, str, str, Optional[str], Dict[str, Any], int, int, int, int, int, int, int) -> None
        """Adds the given primitive instance to this layout.

        Parameters
        ----------
        lib : str
            library name.
        cell : str
            cell name.
        view : str
            view name.
        inst_name : Optional[str]
            optional instance name.
        params : Dict[str, Any]
            parameteric instance parameters.
        dx : int
            X coordinate.
        dy : int
            Y coordinate.
        ocode : int
            Orientation code.
        loc : Tuple[int, int]
            instance location.
        orient : str
            instance orientation.
        nx : int
            number of columns.
        ny : int
            number of rows.
        spx : int
            column pitch.
        spy : int
            row pitch.

        Returns
        -------
        inst : PyLayInstance
            the instance object
        """
        lib = lib.encode(self._encoding)
        cell = cell.encode(self._encoding)
        view = view.encode(self._encoding)
        if inst_name is not None:
            inst_name = inst_name.encode(self._encoding)

        cdef inst_iter_t ref = deref(self._ptr).add_prim_instance(lib, cell, view, inst_name,
                                                                  transformation(dx, dy, ocode),
                                                                  nx, ny, spx, spy)

        # set pcell parameters
        for key, val in params.items():
            key = key.encode(self.encoding)
            if isinstance(val, str):
                val = val.encode(self.encoding)
                deref(ref).second.set_string_param(key, val)
            elif isinstance(val, numbers.Integral):
                deref(ref).second.set_int_param(key, val)
            elif isinstance(val, numbers.Real):
                deref(ref).second.set_double_param(key, val)
            elif isinstance(val, bool):
                deref(ref).second.set_bool_param(key, val)
            else:
                raise ValueError('Unsupported value for key {}: {}'.format(key, val))

    cdef PyLayInstance _add_cinst(self, grid, master, lib_name, PyLayCellView cv,
                                  const char* inst_name, transformation xform,
                                  int nx, int ny, int spx, int spy):
        cdef cellview* template = cv._ptr.get()
        cdef PyLayInstance ans = PyLayInstance(grid, master, lib_name)
        ans._ptr = deref(self._ptr).add_instance(template, inst_name, xform, nx, ny, spx, spy)

        return ans

    def add_instance(self, grid, master, lib_name, inst_name, int dx, int dy, int ocode, int nx,
                     int ny, int spx, int spy):
        # type: (RoutingGrid, TemplateBase, str, Optional[str], int, int, int, int, int, int, int) -> PyLayInstance
        """Adds the given instance to this layout.

        Parameters
        ----------
        grid : RoutingGrid
            the routing grid object.
        master : TemplateBase
            the template master object.
        lib_name : str
            the layout library name.
        inst_name : Optional[str]
            optional instance name.
        dx : int
            X coordinate.
        dy : int
            Y coordinate.
        ocode : int
            orientation code.
        loc : Tuple[int, int]
            instance location.
        orient : str
            instance orientation.
        nx : int
            number of columns.
        ny : int
            number of rows.
        spx : int
            column pitch.
        spy : int
            row pitch.
        """
        if inst_name is not None:
            inst_name = inst_name.encode(self._encoding)

        return self._add_cinst(grid, master, lib_name, master._layout, inst_name,
                               transformation(dx, dy, ocode), nx, ny, spx, spy)

    def add_rect(self, layer, bbox):
        # type: (Union[str, Tuple[str, str]], BBox) -> PyRect
        """Add a new rectangle.

        Parameters
        ----------
        layer : Union[str, Tuple[str, str]]
            the rectangle layer.
        bbox : BBox
            the rectangle bounding box.

        Returns
        -------
        rect : PyRect
            the rectangle object.
        """
        if isinstance(layer, str):
            purpose = None
            layer = layer[0].encode(self._encoding)
        else:
            purpose = layer[1].encode(self._encoding)
            layer = layer[0].encode(self._encoding)

        cdef PyRect ans = PyRect()
        ans._ref = deref(self._ptr).add_rect(layer, purpose, bbox.xl, bbox.yl, bbox.xh, bbox.yh)
        return ans
