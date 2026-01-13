/*
  01.Basics - NoteOnOff
  
  Sends a MIDI Note On message followed by a Note Off message
  every second. This is the "Hello World" of MIDI.

  NON-BLOCKING VERSION:
  Using millis() instead of delay() ensures that USBMIDI.poll() 
  can run continuously to handle incoming data efficiently.
*/

#include <USBMIDI.h>

unsigned long lastToggleTime = 0;
bool noteIsOn = false;

// Config
const unsigned long INTERVAL = 500; // 500ms
const int CHANNEL = 0;
const int NOTE = 60; // Middle C

void setup() {
  USBMIDI.begin();
}

void loop() {
  // 1. Critical: Keep USB communication alive
  USBMIDI.poll();

  // 2. Non-blocking Logic
  unsigned long now = millis();

  if (now - lastToggleTime >= INTERVAL) {
    lastToggleTime = now;

    if (!noteIsOn) {
      // Turn Note ON
      USBMIDI.sendNoteOn(CHANNEL, NOTE, 127);
      noteIsOn = true;
    } else {
      // Turn Note OFF
      USBMIDI.sendNoteOff(CHANNEL, NOTE, 0);
      noteIsOn = false;
    }
  }
}