#pragma once

#define I2C_BTN_PLAY 0x2A
#define I2C_BTN_STOP_INT 0x29
#define I2C_REC_INT 0x27
#define I2C_BTN_QUANTISE_INT 0x26
#define I2C_BTN_METRONOME_INT 0x25
#define I2C_BTN_OVERDUB_INT 0x24

#include <LEDRingSmall.h>
#define I2C_LED_RING ISSI3746_SJ2 | ISSI3746_SJ7