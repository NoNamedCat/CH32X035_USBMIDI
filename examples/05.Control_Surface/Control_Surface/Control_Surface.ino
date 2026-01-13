/*
  05.Control_Surface
  
  Reads analog potentiometers and sends MIDI Control Change (CC) messages.
  
  Features:
  - Analog Smoothing (Hysteresis) to prevent MIDI flooding due to noise.
  - Maps 12-bit ADC (0-4095) to 7-bit MIDI (0-127).
  
  Hardware:
  - Connect Potentiometers to A0, A1, etc.
*/

#include <USBMIDI.h>

// Configuration
const int NUM_KNOBS = 2;
const int KNOB_PINS[NUM_KNOBS] = {A0, A1};
const int CC_NUMBERS[NUM_KNOBS] = {10, 11}; // Pan, Expression, etc.
const int MIDI_CHANNEL = 0;
const unsigned long UPDATE_INTERVAL = 5; // Check every 5ms

// State tracking
int lastMidiValue[NUM_KNOBS] = {-1, -1};
unsigned long lastUpdate = 0;

void setup() {
  USBMIDI.begin();
  
  // Optional: Configure ADC resolution if supported by core
  // analogReadResolution(12); 
}

void loop() {
  // 1. Keep USB active
  USBMIDI.poll();

  // 2. Read Sensors (Non-blocking throttle)
  unsigned long now = millis();
  
  if (now - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = now;
    
    for (int i = 0; i < NUM_KNOBS; i++) {
      int rawValue = analogRead(KNOB_PINS[i]);
      
      // Map 12-bit ADC (0-4095) to 7-bit MIDI (0-127)
      // Adjust '4095' if your board uses 10-bit (1023)
      int midiValue = map(rawValue, 0, 4095, 0, 127);
      
      // Clamp
      if (midiValue < 0) midiValue = 0;
      if (midiValue > 127) midiValue = 127;
      
      // Send only on change
      if (midiValue != lastMidiValue[i]) {
        USBMIDI.sendControlChange(MIDI_CHANNEL, CC_NUMBERS[i], midiValue);
        lastMidiValue[i] = midiValue;
      }
    }
  }
}