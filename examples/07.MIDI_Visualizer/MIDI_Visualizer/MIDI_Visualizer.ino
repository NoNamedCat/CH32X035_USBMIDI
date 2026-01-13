/*
  07.MIDI_Visualizer
  
  Visualizes incoming MIDI Notes using LEDs.
  Useful for debugging or creating light shows synced to music.
  
  Hardware:
  - LED connected to PC0 (or any PWM capable pin).
*/

#include <USBMIDI.h>

// Pin to flash
const int LED_PIN = PC0; // Standard LED on many CH32 boards, adjust if needed

void setup() {
  USBMIDI.begin();
  pinMode(LED_PIN, OUTPUT);
  
  // Register callbacks
  USBMIDI.setHandleNoteOn(handleNoteOn);
  USBMIDI.setHandleNoteOff(handleNoteOff);
  USBMIDI.setHandleControlChange(handleCC);
}

void loop() {
  USBMIDI.poll();
}

void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  // Option 1: Simple On/Off
  // digitalWrite(LED_PIN, LOW); // Active Low
  
  // Option 2: PWM Brightness based on Velocity (if supported by pin)
  // Map velocity 0-127 to PWM 0-255
  int brightness = map(velocity, 0, 127, 0, 255);
  // analogWrite(LED_PIN, brightness); // Uncomment if using Arduino Core with PWM support
  
  // For now, simple logic:
  if (velocity > 0) {
     digitalWrite(LED_PIN, LOW); // Turn On (Active Low common on dev boards)
  } else {
     digitalWrite(LED_PIN, HIGH); // Turn Off
  }
}

void handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  digitalWrite(LED_PIN, HIGH); // Turn Off
}

void handleCC(uint8_t channel, uint8_t control, uint8_t value) {
  // Example: CC 1 (Modulation Wheel) controls LED brightness manually
  if (control == 1) {
    // analogWrite(LED_PIN, map(value, 0, 127, 0, 255));
  }
}
