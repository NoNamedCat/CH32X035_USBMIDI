/*
  03.Advanced_Controls - PitchBend_Aftertouch
  
  Demonstrates sending continuous high-resolution data.
  Sweeps the Pitch Bend up and down and sends Aftertouch messages.
*/

#include <USBMIDI.h>

void setup() {
  USBMIDI.begin();
}

void loop() {
  // Sweep Pitch Bend Up
  for (int i = -8192; i < 8191; i += 128) {
    USBMIDI.sendPitchBend(0, i);
    delay(5);
  }
  
  // Sweep Pitch Bend Down
  for (int i = 8191; i > -8192; i -= 128) {
    USBMIDI.sendPitchBend(0, i);
    delay(5);
  }
  
  // Reset Center
  USBMIDI.sendPitchBend(0, 0);
  delay(500);
  
  // Send Channel Aftertouch (Pressure)
  USBMIDI.sendNoteOn(0, 64, 100);
  for (int i = 0; i < 127; i++) {
    USBMIDI.sendAfterTouch(0, i); // Increase pressure
    delay(10);
  }
  USBMIDI.sendNoteOff(0, 64, 0);
  USBMIDI.sendAfterTouch(0, 0); // Reset pressure
  
  delay(1000);
  USBMIDI.poll();
}
