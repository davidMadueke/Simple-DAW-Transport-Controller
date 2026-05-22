#include <Arduino.h>
#include <unity.h>
#include <Wire.h>
#include "deviceNames.h"
#include <MIDI.h>
#include <Adafruit_TinyUSB.h>

void setUp(void)
{}

void tearDown(void)
{}

void test_led_builtin_pin_number(void)
{
  TEST_ASSERT_EQUAL(13, LED_BUILTIN);
}


Adafruit_USBD_MIDI usb_midi;

MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

// Variable that holds the current position in the sequence.
uint32_t position = 0;

// Store example melody as an array of note values
byte note_sequence[] = {
    74, 78, 81, 86, 90, 93, 98, 102, 57, 61, 66, 69, 73, 78, 81, 85, 88, 92, 97, 100, 97, 92, 88, 85, 81, 78,
    74, 69, 66, 62, 57, 62, 66, 69, 74, 78, 81, 86, 90, 93, 97, 102, 97, 93, 90, 85, 81, 78, 73, 68, 64, 61,
    56, 61, 64, 68, 74, 78, 81, 86, 90, 93, 98, 102
};

void handleNoteOn(byte channel, byte pitch, byte velocity) {
  // Log when a note is pressed.
  Serial.print("Note on: channel = ");
  Serial.print(channel);

  Serial.print(" pitch = ");
  Serial.print(pitch);

  Serial.print(" velocity = ");
  Serial.println(velocity);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  // Log when a note is released.
  Serial.print("Note off: channel = ");
  Serial.print(channel);

  Serial.print(" pitch = ");
  Serial.print(pitch);

  Serial.print(" velocity = ");
  Serial.println(velocity);
}

void setup() {
// Manual begin() is required on core without built-in support e.g. mbed rp2040
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  Serial.begin(115200);

  usb_midi.setStringDescriptor(USB_DEVICE_NAME);

  // Initialize MIDI, and listen to all MIDI channels
  // This will also call usb_midi's begin()
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
  // Attach the handleNoteOn function to the MIDI Library. It will
  // be called whenever the Bluefruit receives MIDI Note On messages.
  MIDI.setHandleNoteOn(handleNoteOn);

  // Do the same for MIDI Note Off messages.
  MIDI.setHandleNoteOff(handleNoteOff);
}

void loop() {

  // not enumerated()/mounted() yet: nothing to do
  if (!TinyUSBDevice.mounted()) {
    return;
  }

  static uint32_t start_ms = 0;
  if (millis() - start_ms > 266) {
    start_ms += 266;

    // Setup variables for the current and previous
    // positions in the note sequence.
    int previous = position - 1;

    // If we currently are at position 0, set the
    // previous position to the last note in the sequence.
    if (previous < 0) {
      previous = sizeof(note_sequence) - 1;
    }

    // Send Note On for current position at full velocity (127) on channel 1.
    MIDI.sendNoteOn(note_sequence[position]+1, 127, 1);

    // Send Note Off for previous note.
    MIDI.sendNoteOff(note_sequence[previous]+1, 0, 1);

    // Increment position
    position++;

    // If we are at the end of the sequence, start over.
    if (position >= sizeof(note_sequence)) {
      position = 0;
    }
  }

  while (MIDI.read()) {
     // controllers must call .read() to keep the queue clear even if they are not responding to MIDI
  }
}

