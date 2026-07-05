
#include <freertos/queue.h>
#pragma once

struct InputSource
{
    QueueHandle_t queue;
    enum Type { RgbButton, TapTempo, InfScrollEncoder, VolEncoder } type;
};