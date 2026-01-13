/*
  06.Drum_Controller
  
  Reads digital buttons and sends MIDI Note On/Off messages.
  Perfect for creating a MIDI Drum Pad or Clip Launcher.
  
  Features:
  - Software Debouncing to prevent double-triggering.
  - Active LOW logic (internal pullups used).
  
  Hardware:
  - Connect Buttons between Pins and GND.
*/

#include <USBMIDI.h>

// Configuration structure for a button
struct MidiButton {
  int pin;
  int note;
  int state;             // Current state (HIGH/LOW)
  int lastState;         // Last read state
  unsigned long lastDebounceTime;
};

// Global settings
const unsigned long DEBOUNCE_DELAY = 10; // ms
const int MIDI_CHANNEL = 9; // Channel 10 is standard for Drums (0-indexed = 9)
const int VELOCITY = 127;

// Define your buttons here
MidiButton buttons[] = {
  {0, 36, HIGH, HIGH, 0}, // Pin 0 -> Kick (Note 36)
  {1, 38, HIGH, HIGH, 0}, // Pin 1 -> Snare (Note 38)
  {2, 42, HIGH, HIGH, 0}  // Pin 2 -> Closed Hi-Hat (Note 42)
};

const int NUM_BUTTONS = sizeof(buttons) / sizeof(MidiButton);

void setup() {
  USBMIDI.begin();
  
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
}

void loop() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    readButton(i);
  }
  USBMIDI.poll();
}

void readButton(int index) {
  int reading = digitalRead(buttons[index].pin);

  // If the switch changed, due to noise or pressing:
  if (reading != buttons[index].lastState) {
    buttons[index].lastDebounceTime = millis();
  }

  if ((millis() - buttons[index].lastDebounceTime) > DEBOUNCE_DELAY) {
    // If the button state has changed:
    if (reading != buttons[index].state) {
      buttons[index].state = reading;

      // Active LOW (Input Pullup): LOW means Pressed
      if (buttons[index].state == LOW) {
        USBMIDI.sendNoteOn(MIDI_CHANNEL, buttons[index].note, VELOCITY);
      } else {
        USBMIDI.sendNoteOff(MIDI_CHANNEL, buttons[index].note, 0);
      }
    }
  }

  buttons[index].lastState = reading;
}
