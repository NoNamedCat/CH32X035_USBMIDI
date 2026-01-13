/*
  02.Input_Callbacks - AllCallbacks
  
  Demonstrates how to receive various MIDI messages.
  This example acts as a MIDI Monitor/Echo device.
  Ideally, use a Serial monitor to print results, but CH32X035 
  USB is occupied by MIDI, so we will just echo them back 
  modified (e.g. +1 octave) to prove it works.
*/

#include <USBMIDI.h>

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  // Echo back one octave higher
  if (note < 116) 
    USBMIDI.sendNoteOn(channel, note + 12, velocity);
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  if (note < 116) 
    USBMIDI.sendNoteOff(channel, note + 12, velocity);
}

void onControlChange(uint8_t channel, uint8_t control, uint8_t value) {
  // Invert the value and send back
  USBMIDI.sendControlChange(channel, control, 127 - value);
}

void onPitchBend(uint8_t channel, int value) {
  // Echo pitch bend
  USBMIDI.sendPitchBend(channel, value);
}

void onProgramChange(uint8_t channel, uint8_t program) {
  // Echo program change
  USBMIDI.sendProgramChange(channel, program);
}

void setup() {
  USBMIDI.begin();
  
  USBMIDI.setHandleNoteOn(onNoteOn);
  USBMIDI.setHandleNoteOff(onNoteOff);
  USBMIDI.setHandleControlChange(onControlChange);
  USBMIDI.setHandlePitchBend(onPitchBend);
  USBMIDI.setHandleProgramChange(onProgramChange);
}

void loop() {
  // Crucial: Must poll frequently to receive data
  USBMIDI.poll();
}
