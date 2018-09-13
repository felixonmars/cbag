# distutils: language = c++

from cellview cimport *

from cython.operator cimport dereference as deref

import numbers

cdef extern from "<utility>" namespace "std" nogil:
    cdef unique_ptr[tech] move(unique_ptr[tech])


# initialize logging
init_logging()


cdef class PyTech:
    def __init__(self, tech_params, encoding):
        cdef unordered_map[string, lay_t] lay_map
        cdef unordered_map[string, purp_t] pur_map
        cdef unordered_map[string, pair[lay_t, lay_t]] via_map
        cdef pair[lay_t, lay_t] tmp_pair

        for key, val in tech_params['layer'].items():
            lay_map[key.encode(encoding)] = val
        for key, val in tech_params['purpose'].items():
            pur_map[key.encode(encoding)] = val
        for key, val in tech_params['via_layers'].items():
            tmp_pair.first = val[0]
            tmp_pair.second = val[1]
            via_map[key.encode(encoding)] = tmp_pair

        options = tech_params['options']
        self._ptr.reset(new tech(lay_map, pur_map, via_map, options['default_purpose'].encode(encoding),
                                 options['pin_purpose'].encode(encoding), options['make_pin_obj']))

    def __dealloc__(self):
        self._ptr.reset()


cdef class PyRect:
    def __init__(self):
        pass


cdef class PyPolygon:
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
    def __init__(self, PyTech tech, cell_name, encoding, int geo_mode=0):
        self._encoding = encoding
        tmp = cell_name.encode(encoding)
        self._ptr.reset(new cellview(tech._ptr.get(), tmp, geo_mode))

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

    def add_pin(self, net, layer, bbox, label):
        # type: (str, str, BBox, str) -> None
        """Add a new pin.

        Parameters
        ----------
        net : str
            the pin net name.
        layer : str
            the pin layer name.
        bbox : BBox
            the pin bounding box.
        label : str
            the pin label.
        """
        net = net.encode(self._encoding)
        layer = layer.encode(self._encoding)
        label = label.encode(self._encoding)

        deref(self._ptr).add_pin(layer, bbox.xl, bbox.yl, bbox.xh, bbox.yh, net, label)

    def add_via(self, via_id, int dx, int dy, int ocode, int w, int h, int nx, int ny, int spx, int spy,
                int enc1x, int enc1y, int enc2x, int enc2y, int off1x, int off1y,
                int off2x, int off2y, cbool add_layers=False):
        via_id = via_id.encode(self._encoding)

        cdef uint32_t num[2]
        cdef dist_t cut_dim[2]
        cdef offset_t cut_sp[2]
        cdef offset_t enc1[2]
        cdef offset_t enc2[2]
        cdef offset_t off1[2]
        cdef offset_t off2[2]

        num[:] = [nx, ny]
        cut_dim[:] = [w, h]
        cut_sp[:] = [spx, spy]
        enc1[:] = [enc1x, enc1y]
        enc2[:] = [enc2x, enc2y]
        off1[:] = [off1x, off1y]
        off2[:] = [off2x, off2y]

        cdef PyVia ans = PyVia()
        ans._ref = deref(self._ptr).add_via(transformation(dx, dy, ocode), via_id, num, cut_dim,
                                            cut_sp, enc1, off1, enc2, off2, add_layers)
        return ans
