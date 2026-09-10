from __future__ import absolute_import, print_function, unicode_literals
from ableton.v3.control_surface import ElementsBase, MIDI_CC_TYPE, MapMode
from .consts import (
    CC_ARRANGEMENT_ZOOM,
    CC_SONG_POSITION,
    CC_TEMPO_ADJUST,
    MIDI_CHANNEL,
)


class Elements(ElementsBase):
    """Hardware map for the inf-scroll encoder used by the BLE outbound test.

    Each mode is a distinct CC on MIDI channel 1. Absolute map mode is required
    because the protocol encodes direction in the velocity value. Exclusive
    forwarding is EncoderElement's default, so repeated identical velocities
    (slow ticks) still reach the script.
    """

    def __init__(self, *a, **k):
        super().__init__(*a, global_channel=MIDI_CHANNEL, **k)
        encoder_kw = dict(
            msg_type=MIDI_CC_TYPE,
            map_mode=MapMode.Absolute,
            needs_takeover=False,
            is_feedback_enabled=False,
        )
        self.add_encoder(CC_TEMPO_ADJUST, "Tempo_Encoder", **encoder_kw)
        self.add_encoder(CC_SONG_POSITION, "Song_Position_Encoder", **encoder_kw)
        self.add_encoder(CC_ARRANGEMENT_ZOOM, "Arrangement_Zoom_Encoder", **encoder_kw)
