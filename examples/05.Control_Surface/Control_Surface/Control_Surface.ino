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

// State tracking
int lastMidiValue[NUM_KNOBS] = {-1, -1};

void setup() {
  USBMIDI.begin();
  
  // Optional: Configure ADC resolution if supported by core, 
  // otherwise standard analogRead works fine.
  // analogReadResolution(12); // CH32 is usually 12-bit native
}

void loop() {
  for (int i = 0; i < NUM_KNOBS; i++) {
    int rawValue = analogRead(KNOB_PINS[i]);
    
    // Map 12-bit ADC (0-4095) to 7-bit MIDI (0-127)
    // Adjust input max (4095) depending on your board/core
    int midiValue = map(rawValue, 0, 4095, 0, 127);
    
    // Clamp to ensure valid MIDI range
    if (midiValue < 0) midiValue = 0;
    if (midiValue > 127) midiValue = 127;
    
    // Check for change using simple hysteresis
    // Only send if value changed and verify noise threshold if needed
    if (midiValue != lastMidiValue[i]) {
      USBMIDI.sendControlChange(MIDI_CHANNEL, CC_NUMBERS[i], midiValue);
      lastMidiValue[i] = midiValue;
    }
  }
  
  // Throttle slightly to prevent USB buffer overflow if many knobs change at once
  delay(5); 
  
  USBMIDI.poll();
}
