"""Ableton Live 12+ MIDI Remote Script for the inf-scroll encoder BLE test.

Copy this folder to:

    Windows  %USERPROFILE%\\Documents\\Ableton\\User Library\\Remote Scripts\\EndlessEncoder
    macOS    ~/Music/Ableton/User Library/Remote Scripts/EndlessEncoder

Then restart Live and select EndlessEncoder as a Control Surface, with the
BLE MIDI port as Input (and Output if you want feedback).
"""
from __future__ import absolute_import, print_function, unicode_literals
from ableton.v3.control_surface.capabilities import (
    NOTES_CC,
    PORTS_KEY,
    SCRIPT,
    inport,
    outport,
)
from .EndlessEncoder import EndlessEncoder


def get_capabilities():
    return {
        PORTS_KEY: [
            inport(props=[NOTES_CC, SCRIPT]),
            outport(props=[NOTES_CC, SCRIPT]),
        ]
    }


def create_instance(c_instance):
    return EndlessEncoder(c_instance=c_instance)
