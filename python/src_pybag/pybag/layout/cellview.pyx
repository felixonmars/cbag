# distutils: language = c++

from cellview cimport *

from cython.operator cimport dereference as deref

# initialize logging
init_logging()


cdef class PyLayInstance:
    def __init__(self, master):
        self._master = master


cdef class PyLayCellView:
    def __init__(self, tech_name, pin_purpose, def_purpose, layer_map, purpose_map,
                 encoding, int geo_mode=0):
        tech_name = tech_name.encode(encoding)

        self._pin_purpose = pin_purpose
        self._def_purpose = def_purpose
        self._layer_map = layer_map
        self._purpose_map = purpose_map
        self._encoding = encoding
        self._ptr.reset(new cellview(tech_name, geo_mode))

    def __dealloc__(self):
        self._ptr.reset()

    @property
    def pin_purpose(self):
        # type: () -> str
        return self._pin_purpose

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
            lay_id = self._layer_map[layer]
            purp_id = self._purpose_map[self._def_purpose]
        else:
            lay_id = self._layer_map[layer[0]]
            purp_id = self._purpose_map[layer[1]]

        rectangle r = deref(self._ptr).get_bbox(lay_id, purp_id)
        return BBox(r.xl(), r.yl(), r.xh(), r.yh())
    
    def get_masters_set(self):
        """Returns a set of all template master keys used in this layout."""
        # TODO: implement this
        return []

    def add_prim_instance(self, lib, cell, view, inst_name=None, loc=(0, 0), orient='R0',
                     int nx=1, int ny=1, int spx=0, int spy=0, unit_mode=True):
        # type: (str, str, str, Optional[str], Tuple[int, int], str, int, int, int, int, bool) -> PyLayInstance
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
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        inst : PyLayInstance
            the instance object
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')

        lib = lib.encode(self._encoding)
        cell = cell.encode(self._encoding)
        view = view.encode(self._encoding)
        if inst_name is not None:
            inst_name = inst_name.encode(self._encoding)

        cdef PyLayInstance ans = PyLayInstance(None)
        ans._ptr = deref(self._ptr).add_prim_instance(lib, cell, view, inst_name,
                                                      make_transform(loc, orient),
                                                      nx, ny, spx, spy)
        return ans

    cdef PyLayInstance _add_cinst(self, master, PyLayCellView cv, const char* inst_name, transformation xform,
                                  int nx, int ny, int spx, int spy):
        cdef cellview* template = cv._ptr.get()
        cdef PyLayInstance ans = PyLayInstance(master)
        ans._ptr = deref(self._ptr).add_instance(template, inst_name, xform, nx, ny, spx, spy)

        return ans

    def add_instance(self, master, inst_name=None, loc=(0, 0), orient='R0',
                     int nx=1, int ny=1, int spx=0, int spy=0, unit_mode=True):
        # type: (TemplateBase, Optional[str], Tuple[int, int], str, int, int, int, int, bool) -> PyLayInstance
        """Adds the given instance to this layout.

        Parameters
        ----------
        master : TemplateBase
            the template master object.
        inst_name : Optional[str]
            optional instance name.
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
        unit_mode : bool
            deprecated parameter.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')

        if inst_name is not None:
            inst_name = inst_name.encode(self._encoding)

        cdef char* cname = inst_name

        return self._add_cinst(master, master._layout, cname, make_transform(loc, orient), nx, ny,
                               spx, spy)
