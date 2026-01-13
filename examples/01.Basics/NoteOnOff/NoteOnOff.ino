/*
  01.Basics - NoteOnOff
  
  Sends a MIDI Note On message followed by a Note Off message
  every second. This is the "Hello World" of MIDI.
*/

#include <USBMIDI.h>

void setup() {
  USBMIDI.begin();
}

void loop() {
  // Play Middle C (Note 60) on Channel 1 (0) with Velocity 127 (Max)
  USBMIDI.sendNoteOn(0, 60, 127);
  delay(500);
  
  // Stop the note
  USBMIDI.sendNoteOff(0, 60, 0);
  delay(500);
  
  // You can also use USBMIDI.poll() here if you expect to receive data,
  // but for a pure sender it's not strictly mandatory (though good practice).
  USBMIDI.poll();
}
