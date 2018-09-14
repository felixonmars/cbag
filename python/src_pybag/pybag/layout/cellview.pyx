# distutils: language = c++

from .util cimport BBox
from cellview cimport *

from cython.operator cimport dereference as deref

import numbers

cdef extern from "<utility>" namespace "std" nogil:
    cdef unique_ptr[tech] move(unique_ptr[tech])
    cdef pt_vector move(pt_vector)

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

    @property
    def bottom_box(self):
        cdef rectangle r = self._ref.value().bot_box()
        return BBox(r.xl(), r.yl(), r.xh(), r.yh())

    @property
    def top_box(self):
        cdef rectangle r = self._ref.value().bot_box()
        return BBox(r.xl(), r.yl(), r.xh(), r.yh())


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

    @property
    def master(self):
        return self._master

    def translate_master_box(self, BBox cur_box):
        return cur_box._transform(deref(self._ptr).second.xform)

cdef void _get_via_enc_offset(int encl, int encr, int enct, int encb, int& encx, int& ency,
                              int& offx, int& offy):
    (&encx)[0] = (encl + encr) // 2
    (&ency)[0] = (enct + encb) // 2
    (&offx)[0] = (encr - encl) // 2
    (&offy)[0] = (enct - encb) // 2

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

    @property
    def cell_name(self):
        # type: () -> str
        return deref(self._ptr).cell_name.decode(self._encoding)
    
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
        cdef char* purpose = NULL
        if isinstance(layer, str):
            layer = layer.encode(self._encoding)
        else:
            tmp = layer[1].encode(self._encoding)
            purpose = tmp
            layer = layer[0].encode(self._encoding)

        cdef rectangle r = deref(self._ptr).get_bbox(layer, purpose)
        return BBox(r.xl(), r.yl(), r.xh(), r.yh())

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
        cdef char* purpose = NULL
        if isinstance(layer, str):
            layer = layer.encode(self._encoding)
        else:
            tmp = layer[1].encode(self._encoding)
            purpose = tmp
            layer = layer[0].encode(self._encoding)

        cdef PyRect ans = PyRect()
        ans._ref = deref(self._ptr).add_rect(layer, purpose, bbox.xl, bbox.yl, bbox.xh, bbox.yh)
        return ans

    def add_rect_arr(self, layer, bbox, int nx, int ny, int spx, int spy):
        # type: (Union[str, Tuple[str, str]], BBox, int, int, int, int) -> None
        """Add an array of rectangles.

        Parameters
        ----------
        layer : Union[str, Tuple[str, str]]
            the rectangle layer.
        bbox : BBox
            the bounding box.
        nx : int
            number of columns.
        ny : int
            number of rows.
        spx : int
            column pitch.
        spy : int
            row pitch.
        """
        cdef char* purp = NULL
        cdef char* lay
        if isinstance(layer, str):
            tmp1 = layer.encode(self._encoding)
            lay = tmp1
        else:
            tmp2 = layer[1].encode(self._encoding)
            purp = tmp2
            tmp1 = layer[0].encode(self._encoding)
            lay = tmp1

        cdef int dx = 0
        cdef int dy = 0
        cdef int xl = bbox.xl
        cdef int yl = bbox.yl
        cdef int xh = bbox.xh
        cdef int yh = bbox.yh
        with nogil:
            for xi in range(nx):
                dy = 0
                for yi in range(ny):
                    deref(self._ptr).add_rect(lay, purp, xl + dx, yl + dy, xh + dx, yh + dy)
                    dy += spy
                dx += spx

    def add_polygon(self, layer, points):
        cdef char* purpose = NULL
        if isinstance(layer, str):
            layer = layer.encode(self._encoding)
        else:
            tmp = layer[1].encode(self._encoding)
            purpose = tmp
            layer = layer[0].encode(self._encoding)

        cdef pt_vector data
        data.reserve(len(points))
        for x, y in points:
            data.emplace_back(x, y)

        cdef PyPolygon ans = PyPolygon()
        ans._ref = deref(self._ptr).add_poly(layer, purpose, move(data))
        return ans

    def add_blockage(self, layer, points, int blk_type):
        if isinstance(layer, str):
            layer = layer.encode(self._encoding)
        else:
            layer = layer[0].encode(self._encoding)

        cdef pt_vector data
        data.reserve(len(points))
        for x, y in points:
            data.emplace_back(x, y)

        cdef PyBlockage ans = PyBlockage()
        ans._ref = deref(self._ptr).add_blockage(layer, blk_type, move(data))
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

    def add_via_arr(self, via_id, penc1, penc2, int dx0, int dy0, int ocode, int w, int h,
                    int nx, int ny, int spx, int spy, int vnx, int vny, int vspx, int vspy,
                    cbool add_layers):
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

        _get_via_enc_offset(penc1[0], penc1[1], penc1[2], penc1[3], enc1[0], enc1[1],
                            off1[0], off1[1])
        _get_via_enc_offset(penc2[0], penc2[1], penc2[2], penc2[3], enc2[0], enc2[1],
                            off2[0], off2[1])

        cdef char* via_id_str = via_id
        cdef int dx = dx0
        cdef int dy = dy0
        cdef PyVia ans = PyVia()
        with nogil:
            ans._ref = deref(self._ptr).add_via(transformation(dx, dy, ocode), via_id_str, num, cut_dim,
                                                cut_sp, enc1, off1, enc2, off2, add_layers)
            dy += spy
            for yi in range(1, ny):
                deref(self._ptr).add_via(transformation(dx, dy, ocode), via_id_str, num, cut_dim,
                                         cut_sp, enc1, off1, enc2, off2, add_layers)
                dy += spy

            dx += spx
            for xi in range(1, nx):
                dy = dy0
                for yi in range(ny):
                    deref(self._ptr).add_via(transformation(dx, dy, ocode), via_id_str, num, cut_dim,
                                             cut_sp, enc1, off1, enc2, off2, add_layers)
                    dy += spy
                dx += spx

        return ans

    def add_path(self, layer, points, int width, int start_style, int stop_style, int join_style):
        cdef char* purpose = NULL
        if isinstance(layer, str):
            layer = layer.encode(self._encoding)
        else:
            tmp = layer[1].encode(self._encoding)
            purpose = tmp
            layer = layer[0].encode(self._encoding)

        cdef int half_w = width // 2
        cdef pt_vector data
        data.reserve(len(points))
        for x, y in points:
            data.emplace_back(x, y)

        cdef PyPath ans = PyPath()
        ans._ref = deref(self._ptr).add_path(layer, purpose, move(data), half_w, start_style,
                                             stop_style, join_style)
        return ans
