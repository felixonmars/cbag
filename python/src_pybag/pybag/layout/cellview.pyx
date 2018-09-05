# distutils: language = c++

from cellview cimport *

from cython.operator cimport dereference as deref

# initialize logging
init_logging()


cdef class PyLayCellView:
    def __init__(self, tech_name, pin_purpose, def_purpose, layer_map, purpose_map,
                 encoding, int geo_mode=0):
        tech_name = tech_name.encode(encoding)

        self._pin_purpose = pin_purpose
        self._def_purpose = def_purpose
        self._layer_map = layer_map
        self._purpose_map = purpose_map
        self._layer_lookup = layer_lookup
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

    def add_instance(self, instance):
        """Adds the given instance to this layout.

        Parameters
        ----------
        instance : bag.layout.objects.Instance
            the instance to add.
        """
        if self._finalized:
            raise Exception('Layout is already finalized.')

        # if isinstance(instance.nx, float) or isinstance(instance.ny, float):
        #     raise Exception('float nx/ny')

        self._inst_list.append(instance)
