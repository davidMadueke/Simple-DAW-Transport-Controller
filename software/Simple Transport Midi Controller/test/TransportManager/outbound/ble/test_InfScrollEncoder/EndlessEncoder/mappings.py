from __future__ import absolute_import, print_function, unicode_literals


def create_mappings(_control_surface):
    mappings = {}
    mappings["Transport"] = dict(
        tempo_encoder="tempo_encoder",
        song_position_encoder="song_position_encoder",
        arrangement_zoom_encoder="arrangement_zoom_encoder",
    )
    return mappings
