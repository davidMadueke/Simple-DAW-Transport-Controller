#pragma once
#include <U8g2lib.h>

// SX1509 Pin Declarations
// GPA pins (Port A)
#define SX1509_PIN0 0
#define SX1509_PIN1 1
#define SX1509_PIN2 2
#define SX1509_PIN3 3
#define SX1509_PIN4 4
#define SX1509_PIN5 5
#define SX1509_PIN6 6
#define SX1509_PIN7 7

// GPB pins (Port B)
#define SX1509_PIN8 8
#define SX1509_PIN9 9
#define SX1509_PIN10 10
#define SX1509_PIN11 11
#define SX1509_PIN12 12
#define SX1509_PIN13 13
#define SX1509_PIN14 14
#define SX1509_PIN15 15

// Pin declarations for the SX1509 GPIO Expander interrupt pins sent to the ESP32
#define SX1509_INT T5

// Encoders Pin declarations
#define PIN_ENC_INF_SCROLL_R SX1509_PIN8
#define PIN_ENC_INF_SCROLL_G SX1509_PIN9
#define PIN_ENC_INF_SCROLL_B SX1509_PIN10
#define PIN_ENC_INF_SCROLL_W1 A0
#define PIN_ENC_INF_SCROLL_W2 A1
#define PIN_ENC_INF_SCROLL_SW SX1509_PIN2

#define PIN_ENC_VOLUME_W1 A2
#define PIN_ENC_VOLUME_W2 A3
#define PIN_ENC_VOLUME_SW SX1509_PIN1

// Button Interrupts Pins
#define PIN_BTN_PLAY_INT T9
#define PIN_BTN_STOP_INT T10
#define PIN_REC_INT T11
#define PIN_BTN_QUANTISE_INT T12
#define PIN_BTN_METRONOME_INT A4
#define PIN_BTN_OVERDUB_INT A5

// TAP Tempo Jack Inputs
#define PIN_JACK_TAP_TEMPO SX1509_GPB0

// "Power" Button
#define PIN_SYS_SW_PWR T6

// "Serial" RX and TX pins
#define PIN_SERIAL_RX RX1
#define PIN_SERIAL_TX TX1

// SPI OLED Display Pins
#define PIN_OLED_SPI_SCK SCK
#define PIN_OLED_SPI_SDA MOSI
#define PIN_OLED_SPI_RES U8X8_PIN_NONE // RES tieded to MCU RESET
#define PIN_OLED_SPI_CS U8X8_PIN_NONE // CS grounded (We expect OLED display is the only SPI slave on bus)
#define PIN_OLED_SPI_DC T13
