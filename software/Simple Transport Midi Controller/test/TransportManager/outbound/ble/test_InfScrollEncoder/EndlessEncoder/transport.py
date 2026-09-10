from __future__ import absolute_import, print_function, unicode_literals
import Live
from ableton.v3.base import clamp
from ableton.v3.control_surface.components import TransportComponent as TransportComponentBase
from ableton.v3.control_surface.controls import InputControl
from ableton.v3.live import move_current_song_time
from .consts import (
    ARRANGEMENT_VIEW_NAME,
    ENDLESS_ENCODER_COARSE_MULTIPLIER,
    SONG_POSITION_PLAYING_MULTIPLIER,
    TEMPO_COARSE_STEP_BPM,
    TEMPO_FINE_STEP_BPM,
    TEMPO_MAX,
    TEMPO_MIN,
    decode_endless_encoder,
)


class TransportComponent(TransportComponentBase):
    """Inf-scroll encoder transport, following MackieControl/Transport.py.

    Tempo and song position use the same Live calls as Mackie's jog wheel
    (`song.tempo` and `move_current_song_time`). Arrangement zoom uses
    `Application.View.zoom_view`, the LOM call behind v3 ZoomComponent's
    horizontal encoder (`set_horizontal_zoom_encoder`). Fine vs coarse comes
    from the endless-encoder velocity band rather than modifier keys.
    """

    # InputControl (not EncoderControl): EncoderControl.normalize_value would
    # turn Absolute CCs into last-value deltas and drop the protocol bands.
    tempo_encoder = InputControl()
    song_position_encoder = InputControl()
    arrangement_zoom_encoder = InputControl()

    @tempo_encoder.value
    def tempo_encoder(self, value, _):
        ticks, is_coarse = decode_endless_encoder(value)
        if ticks == 0:
            return
        step_bpm = TEMPO_COARSE_STEP_BPM if is_coarse else TEMPO_FINE_STEP_BPM
        self.song.tempo = clamp(
            self.song.tempo + ticks * step_bpm,
            TEMPO_MIN,
            TEMPO_MAX,
        )

    @song_position_encoder.value
    def song_position_encoder(self, value, _):
        ticks, is_coarse = decode_endless_encoder(value)
        if ticks == 0:
            return
        step = float(abs(ticks))
        if is_coarse:
            step *= ENDLESS_ENCODER_COARSE_MULTIPLIER
        if self.song.is_playing:
            step *= SONG_POSITION_PLAYING_MULTIPLIER
        move_current_song_time(
            self.song,
            -step if ticks < 0 else step,
            truncate_to_beat=False,
        )

    @arrangement_zoom_encoder.value
    def arrangement_zoom_encoder(self, value, _):
        ticks, is_coarse = decode_endless_encoder(value)
        if ticks == 0:
            return
        nav = Live.Application.Application.View.NavDirection
        direction = nav.right if ticks > 0 else nav.left
        # Third argument is the fine-zoom flag; Mackie passes alt_is_pressed().
        repetitions = abs(int(ticks))
        if is_coarse:
            repetitions *= ENDLESS_ENCODER_COARSE_MULTIPLIER
        for _ in range(max(1, repetitions)):
            self.application.view.zoom_view(
                direction,
                ARRANGEMENT_VIEW_NAME,
                not is_coarse,
            )
