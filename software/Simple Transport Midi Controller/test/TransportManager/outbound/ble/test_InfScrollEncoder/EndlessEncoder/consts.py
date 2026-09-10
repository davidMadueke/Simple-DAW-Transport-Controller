# uncompyle6-style factory scripts keep this import; Live 12's interpreter is Python 3.
from __future__ import absolute_import, print_function, unicode_literals

# Match firmware MIDI_CHANNEL (1-based). EncoderElement channels are 0-based.
MIDI_CHANNEL_1_BASED = 1
MIDI_CHANNEL = MIDI_CHANNEL_1_BASED - 1

# Inf-scroll encoder mode CCs from constants.h INF_SCROLL_MODE_TABLE.
# Encoder-button presses only cycle the device mode; they are not sent as MIDI.
CC_TEMPO_ADJUST = 85
CC_RECORD_QUANTISATION = 86
CC_SONG_POSITION = 87
CC_ARRANGEMENT_ZOOM = 88

# Endless-encoder velocity bands from MIDI_PACKET.h / Midi Protocol Architecture v1.
# Velocity = start + x, x in [0, MAX_DELTA], x = 0 means a physical delta of 1.
ENDLESS_ENCODER_MAX_DELTA = 10
ENDLESS_ENCODER_LEFT_FINE_START = 44
ENDLESS_ENCODER_LEFT_COARSE_START = 88
ENDLESS_ENCODER_RIGHT_FINE_START = 22
ENDLESS_ENCODER_RIGHT_COARSE_START = 100

# Coarse band is n times the fine step (architecture diagram "n").
# n = 10 makes coarse tempo 1.0 BPM when fine is 0.1 BPM (Mackie / v3 Transport).
ENDLESS_ENCODER_COARSE_MULTIPLIER = 10

TEMPO_MIN = 20.0
TEMPO_MAX = 999.0
TEMPO_FINE_STEP_BPM = 0.1
TEMPO_COARSE_STEP_BPM = TEMPO_FINE_STEP_BPM * ENDLESS_ENCODER_COARSE_MULTIPLIER

SONG_POSITION_PLAYING_MULTIPLIER = 4.0
ARRANGEMENT_VIEW_NAME = "Arranger"


def _in_band(value, start):
    return start <= value <= start + ENDLESS_ENCODER_MAX_DELTA


def decode_endless_encoder(value):
    """Decode an endless-encoder CC velocity into (signed_ticks, is_coarse).

    signed_ticks is 0 when the value is outside the four protocol bands.
    Magnitude is (x + 1) so a start-band value means one tick, matching the
    firmware rule that x = 0 means a delta of 1. Coarse scaling is left to
    the caller via ENDLESS_ENCODER_COARSE_MULTIPLIER.
    """
    value = int(value)
    if _in_band(value, ENDLESS_ENCODER_RIGHT_FINE_START):
        return (value - ENDLESS_ENCODER_RIGHT_FINE_START + 1, False)
    if _in_band(value, ENDLESS_ENCODER_LEFT_FINE_START):
        return (-(value - ENDLESS_ENCODER_LEFT_FINE_START + 1), False)
    if _in_band(value, ENDLESS_ENCODER_RIGHT_COARSE_START):
        return (value - ENDLESS_ENCODER_RIGHT_COARSE_START + 1, True)
    if _in_band(value, ENDLESS_ENCODER_LEFT_COARSE_START):
        return (-(value - ENDLESS_ENCODER_LEFT_COARSE_START + 1), True)
    return (0, False)
