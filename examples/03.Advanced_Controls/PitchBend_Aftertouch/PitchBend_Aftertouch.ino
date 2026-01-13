/*
  03.Advanced_Controls - PitchBend_Aftertouch
  
  Demonstrates sending continuous high-resolution data.
  Sweeps the Pitch Bend up and down and sends Aftertouch messages.

  NON-BLOCKING VERSION:
  Uses a state machine to sweep values without freezing the USB stack.
*/

#include <USBMIDI.h>

enum State {
  STATE_BEND_UP,
  STATE_BEND_DOWN,
  STATE_RESET_WAIT,
  STATE_TOUCH_UP,
  STATE_TOUCH_RESET,
  STATE_FINAL_WAIT
};

State currentState = STATE_BEND_UP;
unsigned long lastUpdate = 0;

// Variables for sweeping
int currentBend = -8192;
int currentTouch = 0;

void setup() {
  USBMIDI.begin();
}

void loop() {
  // Always poll USB
  USBMIDI.poll();

  unsigned long now = millis();

  switch (currentState) {
    case STATE_BEND_UP:
      if (now - lastUpdate >= 5) { // Update every 5ms
        lastUpdate = now;
        USBMIDI.sendPitchBend(0, currentBend);
        currentBend += 128;
        if (currentBend >= 8191) {
          currentBend = 8191;
          currentState = STATE_BEND_DOWN;
        }
      }
      break;

    case STATE_BEND_DOWN:
      if (now - lastUpdate >= 5) {
        lastUpdate = now;
        USBMIDI.sendPitchBend(0, currentBend);
        currentBend -= 128;
        if (currentBend <= -8192) {
          currentBend = -8192;
          // Center pitch before moving on
          USBMIDI.sendPitchBend(0, 0);
          currentState = STATE_RESET_WAIT;
        }
      }
      break;

    case STATE_RESET_WAIT:
      if (now - lastUpdate >= 500) { // Wait 500ms
        lastUpdate = now;
        // Prepare for Aftertouch
        USBMIDI.sendNoteOn(0, 64, 100);
        currentTouch = 0;
        currentState = STATE_TOUCH_UP;
      }
      break;

    case STATE_TOUCH_UP:
      if (now - lastUpdate >= 10) { // Update every 10ms
        lastUpdate = now;
        USBMIDI.sendAfterTouch(0, currentTouch);
        currentTouch++;
        if (currentTouch > 127) {
          currentState = STATE_TOUCH_RESET;
        }
      }
      break;

    case STATE_TOUCH_RESET:
        USBMIDI.sendNoteOff(0, 64, 0);
        USBMIDI.sendAfterTouch(0, 0);
        lastUpdate = now;
        currentState = STATE_FINAL_WAIT;
        break;

    case STATE_FINAL_WAIT:
      if (now - lastUpdate >= 1000) { // Wait 1 sec before restarting loop
        lastUpdate = now;
        currentBend = -8192; // Reset variables
        currentState = STATE_BEND_UP;
      }
      break;
  }
}