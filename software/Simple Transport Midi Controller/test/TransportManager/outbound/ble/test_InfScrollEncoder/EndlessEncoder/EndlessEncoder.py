from __future__ import absolute_import, print_function, unicode_literals
import logging
from ableton.v3.control_surface import ControlSurface, ControlSurfaceSpecification
from .elements import Elements
from .mappings import create_mappings
from .transport import TransportComponent

logger = logging.getLogger("EndlessEncoder")


class Specification(ControlSurfaceSpecification):
    elements_type = Elements
    create_mappings_function = create_mappings
    component_map = {"Transport": TransportComponent}
    num_tracks = 1
    num_scenes = 1
    include_auto_arming = False


class EndlessEncoder(ControlSurface):

    def __init__(self, *a, **k):
        super().__init__(Specification, *a, **k)

    def setup(self):
        super().setup()
        logger.info("EndlessEncoder: inf-scroll remote script loaded")
        self.show_message("EndlessEncoder loaded")
