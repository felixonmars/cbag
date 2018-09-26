# distutils: language = c++

from cython.operator cimport dereference as deref

from .util cimport BBox, BBoxArray
from .cellview cimport *

import numbers
from typing import Any, Tuple, Union, Optional, Dict, List, Iterable

from .pyutil import Orientation

cdef extern from "<utility>" namespace "std" nogil:
    cdef unique_ptr[tech] move(unique_ptr[tech])
    cdef shape_ref[rectangle] move(shape_ref[rectangle])
    cdef shape_ref[polygon] move(shape_ref[polygon])
    cdef shape_ref[polygon45_set] move(shape_ref[polygon45_set])
    cdef cv_obj_ref[blockage] move(cv_obj_ref[blockage])
    cdef cv_obj_ref[boundary] move(cv_obj_ref[boundary])
    cdef cv_obj_ref[instance] move(cv_obj_ref[instance])
    cdef cv_obj_ref[via] move(cv_obj_ref[via])
    cdef point_t move(point_t)

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
        self._pin_purpose = options['pin_purpose']
        self._ptr.reset(new tech(lay_map, pur_map, via_map, options['default_purpose'].encode(encoding),
                                 self._pin_purpose.encode(encoding), options['make_pin_obj']))

    def __dealloc__(self):
        self._ptr.reset()

    @property
    def pin_purpose(self):
        return self._pin_purpose


cdef class PyRect:
    def __init__(self):
        pass

    def commit(self):
        self._ref.commit()

cdef class PyPolygon:
    def __init__(self):
        pass

    def commit(self):
        self._ref.commit()

cdef class PyVia:
    def __init__(self):
        pass

    @property
    def bottom_box(self):
        cdef rectangle r = self._ref.obj.bot_box()
        return BBox(r.xl, r.yl, r.xh, r.yh)

    @property
    def top_box(self):
        cdef rectangle r = self._ref.obj.bot_box()
        return BBox(r.xl, r.yl, r.xh, r.yh)

    def commit(self):
        self._ref.commit()

cdef class PyPath:
    def __init__(self):
        pass

    def commit(self):
        self._ref.commit()

cdef class PyBlockage:
    def __init__(self):
        pass

    def commit(self):
        self._ref.commit()

cdef class PyBoundary:
    def __init__(self):
        pass

    def commit(self):
        self._ref.commit()

cdef class PyLayInstance:
    def __init__(self, grid, master, lib_name):
        self._grid = grid
        self._master = master
        self._lib_name = lib_name

    cdef _update_inst_master(self, PyLayCellView cv):
        if self._ref.parent == NULL:
            raise ValueError('Cannot change object after commit')

        self._ref.obj.set_master(cv._ptr.get())

    cdef _translate_master_box_w_array(self, BBox box):
        cdef uint32_t nx = self._ref.obj.nx
        cdef uint32_t ny = self._ref.obj.ny
        cdef offset_t spx = self._ref.obj.spx
        cdef offset_t spy = self._ref.obj.spy
        cdef BBoxArray box_arr = BBoxArray(box, nx=nx, ny=ny, spx=spx, spy=spy)
        return box_arr.get_overall_bbox().c_transform(self._ref.obj.xform)

    @property
    def nx(self):
        # type: () -> int
        """Number of columns."""
        return self._ref.obj.nx

    @nx.setter
    def nx(self, val):
        # type: (int) -> None
        """Sets the number of columns."""
        if self._ref.parent == NULL:
            raise ValueError('Cannot change object after commit')

        self._ref.obj.nx = val

    @property
    def ny(self):
        # type: () -> int
        """Number of rows."""
        return self._ref.obj.ny

    @ny.setter
    def ny(self, val):
        # type: (int) -> None
        """Sets the number of rows."""
        if self._ref.parent == NULL:
            raise ValueError('Cannot change object after commit')

        self._ref.obj.ny = val

    @property
    def spx_unit(self):
        # type: () -> int
        """The column pitch in resolution units."""
        return self._ref.obj.spx

    @spx_unit.setter
    def spx_unit(self, val):
        # type: (int) -> None
        """Sets the new column pitch in resolution units."""
        if self._ref.parent == NULL:
            raise ValueError('Cannot change object after commit')
        if val < 0:
            raise ValueError('Currently does not support negative pitches.')
        self._ref.obj.spx = val

    @property
    def spy_unit(self):
        # type: () -> int
        """The row pitch in resolution units."""
        return self._ref.obj.spy

    @spy_unit.setter
    def spy_unit(self, val):
        # type: (int) -> None
        """Sets the new row pitch in resolution units."""
        if self._ref.parent == NULL:
            raise ValueError('Cannot change object after commit')
        if val < 0:
            raise ValueError('Currently does not support negative pitches.')
        self._ref.obj.spy = val

    @property
    def master(self):
        # type: () -> PyLayCellView
        return self._master

    @property
    def location_unit(self):
        # type: () -> Tuple[int, int]
        cdef coord_t x = 0
        cdef coord_t y = 0
        self._ref.obj.xform.get_location(x, y)
        return x, y

    @location_unit.setter
    def location_unit(self, new_loc):
        # type: (Tuple[int, int]) -> None
        """Sets the instance location."""
        if self._ref.parent == NULL:
            raise ValueError('Cannot change object after commit')

        self._ref.obj.xform.set_location(new_loc[0], new_loc[1])

    @property
    def orientation(self):
        # type: () -> str
        """The instance orientation"""
        return Orientation(self._ref.obj.xform.orient_code()).name

    @orientation.setter
    def orientation(self, val):
        # type: (str) -> None
        """Sets the instance orientation."""
        if self._ref.parent == NULL:
            raise ValueError('Cannot change object after commit')

        self._ref.obj.xform.set_orient_code(Orientation[val].value)

    @property
    def bound_box(self):
        """Returns the overall bounding box of this instance."""
        return self._translate_master_box_w_array(self._master.bound_box)

    @property
    def array_box(self):
        """Returns the array box of this instance."""
        master_box = getattr(self._master, 'array_box', None)  # type: BBox
        if master_box is None:
            raise ValueError('Master template array box is not defined.')

        return self._translate_master_box_w_array(master_box)

    @property
    def fill_box(self):
        """Returns the fill box of this instance."""
        master_box = getattr(self._master, 'fill_box', None)  # type: BBox
        if master_box is None:
            raise ValueError('Master template fill box is not defined.')

        return self._translate_master_box_w_array(master_box)

    def get_item_location(self, row=0, col=0, unit_mode=True):
        # type: (int, int, bool) -> Tuple[int, int]
        """Returns the location of the given item in the array.

        Parameters
        ----------
        row : int
            the item row index.  0 is the bottom-most row.
        col : int
            the item column index.  0 is the left-most column.
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        xo : Union[float, int]
            the item X coordinate.
        yo : Union[float, int]
            the item Y coordinate.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        if row < 0 or row >= self.ny or col < 0 or col >= self.nx:
            raise ValueError('Invalid row/col index: row=%d, col=%d' % (row, col))

        return col * self.spx_unit, row * self.spy_unit

    def get_bound_box_of(self, row=0, col=0):
        # type: (int, int) -> BBox
        """Returns the bounding box of an instance in this mosaic."""
        dx, dy = self.get_item_location(row=row, col=col)
        cdef BBox box = self._master.bound_box
        box = box.c_transform(self._ref.obj.xform)
        return box.move_by(dx, dy)

    def move_by(self, dx=0, dy=0, unit_mode=True):
        # type: (int, int, bool) -> None
        """Move this instance by the given amount.

        Parameters
        ----------
        dx : int
            the X shift.
        dy : int
            the Y shift.
        unit_mode : bool
            deprecated parameter.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')
        if self._ref.parent == NULL:
            raise ValueError('Cannot change object after commit')

        self._ref.obj.xform.move_by(dx, dy)

    def new_master_with(self, **kwargs):
        # type: (Any) -> None
        """Change the master template of this instance.

        This method will get the old master template layout parameters, update
        the parameter values with the given dictionary, then create a new master
        template with those parameters and associate it with this instance.

        Parameters
        ----------
        **kwargs
            a dictionary of new parameter values.
        """
        self._master = self._master.new_template_with(**kwargs)
        self._update_inst_master(self._master)

    def translate_master_box(self, BBox box):
        # type: (BBox) -> BBox
        """Transform the bounding box in master template.

        Parameters
        ----------
        box : BBox
            the BBox in master template coordinate.

        Returns
        -------
        new_box : BBox
            the corresponding BBox in instance coordinate.
        """
        return box.c_transform(self._ref.obj.xform)

    def translate_master_location(self, mloc, unit_mode=True):
        # type: (Tuple[int, int], bool) -> Tuple[int, int]
        """Returns the actual location of the given point in master template.

        Parameters
        ----------
        mloc : Tuple[int, int]
            the location in master coordinate.
        unit_mode : bool
            deprecated parameter.

        Returns
        -------
        loc : Tuple[int, int]
            The actual location.
        """
        if not unit_mode:
            raise ValueError('unit_mode = False not supported.')

        cdef coord_t x = mloc[0]
        cdef coord_t y = mloc[1]
        self._ref.obj.xform.transform(x, y)

        return x, y

    def translate_master_track(self, layer_id, track_idx):
        # type: (int, Union[float, int]) -> Union[float, int]
        """Returns the actual track index of the given track in master template.

        Parameters
        ----------
        layer_id : int
            the layer ID.
        track_idx : Union[float, int]
            the track index.

        Returns
        -------
        new_idx : Union[float, int]
            the new track index.
        """
        dx, dy = self.location_unit
        return self._grid.transform_track(layer_id, track_idx, dx=dx, dy=dy,
                                          orient=self.orientation)

    def get_port(self, name='', row=0, col=0):
        # type: (Optional[str], int, int) -> Port
        """Returns the port object of the given instance in the array.

        Parameters
        ----------
        name : Optional[str]
            the port terminal name.  If None or empty, check if this
            instance has only one port, then return it.
        row : int
            the instance row index.  Index 0 is the bottom-most row.
        col : int
            the instance column index.  Index 0 is the left-most column.

        Returns
        -------
        port : Port
            the port object.
        """
        dx, dy = self.get_item_location(row=row, col=col)
        xshift, yshift = self.location_unit
        loc = (xshift + dx, yshift + dy)
        return self._master.get_port(name).transform(self._grid, loc=loc,
                                                     orient=self.orientation)

    def get_pin(self, name='', row=0, col=0, layer=-1):
        # type: (Optional[str], int, int, int) -> Union[WireArray, BBox]
        """Returns the first pin with the given name.

        This is an efficient method if you know this instance has exactly one pin.

        Parameters
        ----------
        name : Optional[str]
            the port terminal name.  If None or empty, check if this
            instance has only one port, then return it.
        row : int
            the instance row index.  Index 0 is the bottom-most row.
        col : int
            the instance column index.  Index 0 is the left-most column.
        layer : int
            the pin layer.  If negative, check to see if the given port has only one layer.
            If so then use that layer.

        Returns
        -------
        pin : Union[WireArray, BBox]
            the first pin associated with the port of given name.
        """
        return self.get_port(name, row, col).get_pins(layer)[0]

    def port_pins_iter(self, name='', layer=-1):
        # type: (Optional[str], int) -> Iterable[WireArray]
        """Iterate through all pins of all ports with the given name in this instance array.

        Parameters
        ----------
        name : Optional[str]
            the port terminal name.  If None or empty, check if this
            instance has only one port, then return it.
        layer : int
            the pin layer.  If negative, check to see if the given port has only one layer.
            If so then use that layer.

        Yields
        ------
        pin : WireArray
            the pin as WireArray.
        """
        for col in range(self.nx):
            for row in range(self.ny):
                try:
                    port = self.get_port(name, row, col)
                except KeyError:
                    return
                for warr in port.get_pins(layer):
                    yield warr

    def get_all_port_pins(self, name='', layer=-1):
        # type: (Optional[str], int) -> List[WireArray]
        """Returns a list of all pins of all ports with the given name in this instance array.

        This method gathers ports from all instances in this array with the given name,
        then find all pins of those ports on the given layer, then return as list of WireArrays.

        Parameters
        ----------
        name : Optional[str]
            the port terminal name.  If None or empty, check if this
            instance has only one port, then return it.
        layer : int
            the pin layer.  If negative, check to see if the given port has only one layer.
            If so then use that layer.

        Returns
        -------
        pin_list : List[WireArray]
            the list of pins as WireArrays.
        """
        return list(self.port_pins_iter(name=name, layer=layer))

    def port_names_iter(self):
        # type: () -> Iterable[str]
        """Iterates over port names in this instance.

        Yields
        ------
        port_name : str
            name of a port in this instance.
        """
        return self._master.port_names_iter()

    def has_port(self, port_name):
        # type: (str) -> bool
        """Returns True if this instance has the given port."""
        return self._master.has_port(port_name)

    def has_prim_port(self, port_name):
        # type: (str) -> bool
        """Returns True if this instance has the given primitive port."""
        return self._master.has_prim_port(port_name)

    def commit(self):
        self._ref.commit()

cdef void _get_via_enc_offset(int encl, int encr, int enct, int encb, int& encx, int& ency,
                              int& offx, int& offy):
    (&encx)[0] = (encl + encr) // 2
    (&ency)[0] = (enct + encb) // 2
    (&offx)[0] = (encr - encl) // 2
    (&offy)[0] = (enct - encb) // 2


cdef _compress_points(points, vector[point_t]& ans):
    cdef int cur_len = 0
    cdef coord_t x, y
    cdef coord_t lastx, lasty, dx, dy, dx0, dy0

    ans.reserve(len(points))
    for p in points:
        x = p[0]
        y = p[1]
        with nogil:
            if cur_len == 0:
                ans.push_back(move(point_t(x, y)))
                cur_len += 1
            else:
                lastx = ans[cur_len - 1].x()
                lasty = ans[cur_len - 1].y()
                # make sure we don't have duplicate points
                if x != lastx or y != lasty:
                    dx = x - lastx
                    dy = y - lasty
                    if cur_len >= 2:
                        # check for collinearity
                        dx0 = lastx - ans[cur_len - 2].x()
                        dy0 = lasty - ans[cur_len - 2].y()
                        if (dx == 0 and dx0 == 0) or (dx != 0 and dx0 != 0 and
                                                      dy * dx0 == dx * dy0):
                            # collinear, remove middle point
                            ans.pop_back()
                            cur_len -= 1
                    ans.push_back(move(point_t(x, y)))
                    cur_len += 1


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
        return BBox(r.xl, r.yl, r.xh, r.yh)

    def add_prim_instance(self, lib, cell, view, inst_name, params, int dx, int dy, int ocode,
                          int nx, int ny, int spx, int spy):
        # type: (str, str, str, Optional[str], Dict[str, Any], int, int, int, int, int, int, int, bool) -> None
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
        nx : int
            number of columns.
        ny : int
            number of rows.
        spx : int
            column pitch.
        spy : int
            row pitch.
        """
        lib = lib.encode(self._encoding)
        cell = cell.encode(self._encoding)
        view = view.encode(self._encoding)
        if inst_name is not None:
            inst_name = inst_name.encode(self._encoding)

        cdef cv_obj_ref[instance] ref = deref(self._ptr).add_prim_instance(lib, cell, view, inst_name,
                                                                           transformation(dx, dy, ocode),
                                                                           nx, ny, spx, spy, False)

        # set pcell parameters
        for key, val in params.items():
            key = key.encode(self._encoding)
            if isinstance(val, str):
                val = val.encode(self._encoding)
                ref.obj.set_string_param(key, val)
            elif isinstance(val, numbers.Integral):
                ref.obj.set_int_param(key, val)
            elif isinstance(val, numbers.Real):
                ref.obj.set_double_param(key, val)
            elif isinstance(val, bool):
                ref.obj.set_bool_param(key, val)
            else:
                raise ValueError('Unsupported value for key {}: {}'.format(key, val))

        ref.commit()

    cdef PyLayInstance _add_cinst(self, grid, master, lib_name, PyLayCellView cv,
                                  const char* inst_name, transformation xform,
                                  int nx, int ny, int spx, int spy, cbool commit):
        cdef cellview* template = cv._ptr.get()
        cdef PyLayInstance ans = PyLayInstance(grid, master, lib_name)
        ans._ref = move(deref(self._ptr).add_instance(template, inst_name, xform, nx, ny, spx, spy, commit))

        return ans

    def add_instance(self, grid, master, lib_name, inst_name, int dx, int dy, int ocode, int nx,
                     int ny, int spx, int spy, cbool commit=True):
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
        nx : int
            number of columns.
        ny : int
            number of rows.
        spx : int
            column pitch.
        spy : int
            row pitch.
        commit : bool
            True to commit the object immediately.

        Returns
        -------
        inst : PyLayInstance
            The instance object.
        """
        cdef char* inst_name_char
        if inst_name is None:
            inst_name_char = NULL
        else:
            inst_name = inst_name.encode(self._encoding)
            inst_name_char = inst_name

        return self._add_cinst(grid, master, lib_name, master._layout, inst_name_char,
                               transformation(dx, dy, ocode), nx, ny, spx, spy, commit)

    def add_rect(self, layer, BBox bbox, cbool commit=True):
        # type: (Union[str, Tuple[str, str]], BBox, bool) -> PyRect
        """Add a new rectangle.

        Parameters
        ----------
        layer : Union[str, Tuple[str, str]]
            the rectangle layer.
        bbox : BBox
            the rectangle bounding box.
        commit : bool
            True to commit the object immediately.

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
        ans._ref = move(deref(self._ptr).add_rect(layer, purpose, bbox.xl, bbox.yl, bbox.xh,
                                                  bbox.yh, commit))
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
                    deref(self._ptr).add_rect(lay, purp, xl + dx, yl + dy, xh + dx, yh + dy, True)
                    dy += spy
                dx += spx

    def add_polygon(self, layer, points, cbool commit=True):
        cdef char* purpose = NULL
        if isinstance(layer, str):
            layer = layer.encode(self._encoding)
        else:
            tmp = layer[1].encode(self._encoding)
            purpose = tmp
            layer = layer[0].encode(self._encoding)

        cdef vector[point_t] data
        _compress_points(points, data)

        cdef PyPolygon ans = PyPolygon()
        ans._ref = move(deref(self._ptr).add_poly(layer, purpose, data, commit))
        return ans

    def add_blockage(self, layer, points, int blk_type, cbool commit=True):
        if isinstance(layer, str):
            layer = layer.encode(self._encoding)
        else:
            layer = layer[0].encode(self._encoding)

        cdef vector[point_t] data
        _compress_points(points, data)

        cdef PyBlockage ans = PyBlockage()
        ans._ref = move(deref(self._ptr).add_blockage(layer, blk_type, data, commit))
        return ans

    def add_boundary(self, points, int bnd_type, cbool commit=True):
        cdef vector[point_t] data
        _compress_points(points, data)

        cdef PyBoundary ans = PyBoundary()
        ans._ref = move(deref(self._ptr).add_boundary(bnd_type, data, commit))
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
                    cbool add_layers, cbool commit=True):
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
            ans._ref = move(deref(self._ptr).add_via(transformation(dx, dy, ocode), via_id_str, num, cut_dim,
                                                     cut_sp, enc1, off1, enc2, off2, add_layers, commit))
            dy += spy
            for yi in range(1, ny):
                deref(self._ptr).add_via(transformation(dx, dy, ocode), via_id_str, num, cut_dim,
                                         cut_sp, enc1, off1, enc2, off2, add_layers, True)
                dy += spy

            dx += spx
            for xi in range(1, nx):
                dy = dy0
                for yi in range(ny):
                    deref(self._ptr).add_via(transformation(dx, dy, ocode), via_id_str, num, cut_dim,
                                             cut_sp, enc1, off1, enc2, off2, add_layers, True)
                    dy += spy
                dx += spx

        return ans

    def add_path(self, layer, points, int width, int start_style, int stop_style, int join_style,
                 cbool commit=True):
        cdef char* purpose = NULL
        if isinstance(layer, str):
            layer = layer.encode(self._encoding)
        else:
            tmp = layer[1].encode(self._encoding)
            purpose = tmp
            layer = layer[0].encode(self._encoding)

        cdef int half_w = width // 2
        cdef vector[point_t] data
        _compress_points(points, data)

        cdef PyPath ans = PyPath()
        ans._ref = move(deref(self._ptr).add_path(layer, purpose, data, half_w, start_style,
                                                  stop_style, join_style, commit))
        return ans

    def add_path45_bus(self, layer, points, widths, spaces, int start_style, int stop_style,
                       int join_style, cbool commit=True):
        cdef char* purpose = NULL
        if isinstance(layer, str):
            layer = layer.encode(self._encoding)
        else:
            tmp = layer[1].encode(self._encoding)
            purpose = tmp
            layer = layer[0].encode(self._encoding)

        cdef vector[offset_t] w_vec = widths
        cdef vector[offset_t] sp_vec = spaces
        cdef vector[point_t] data
        _compress_points(points, data)

        cdef PyPath ans = PyPath()
        ans._ref = move(deref(self._ptr).add_path45_bus(layer, purpose, data, w_vec, sp_vec, start_style,
                                                        stop_style, join_style, commit))
        return ans
