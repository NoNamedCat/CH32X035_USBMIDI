/*
  04.USB_MIDI_Bridge
  
  Converts this device into a USB <-> Serial MIDI Bridge.
  
  - Incoming USB MIDI messages are sent out via Hardware Serial (UART).
  - Incoming Serial MIDI messages (DIN-5) are sent to the PC via USB.
  
  HARDWARE REQUIRED:
  - MIDI OUT Circuit: 220 Ohm resistors on TX pin.
  - MIDI IN Circuit: Optocoupler (6N138/PC900) on RX pin. DO NOT CONNECT DIRECTLY.
*/

#include <USBMIDI.h>

// =================================================================================
// USER CONFIGURATION: SELECT YOUR SERIAL PORT
// =================================================================================

// Option 1: Use USART1 (Pins PA9 TX / PA10 RX usually)
#define MIDI_SERIAL Serial

// Option 2: Use USART2 (Pins PA2 TX / PA3 RX usually)
// #define MIDI_SERIAL Serial1

// Option 3: Use USART3 (Check pinout)
// #define MIDI_SERIAL Serial2

// =================================================================================

void setup() {
  // Initialize USB MIDI
  USBMIDI.begin();
  
  // Initialize Hardware MIDI (Standard Baud Rate 31250)
  MIDI_SERIAL.begin(31250);
  
  // Register callbacks for USB -> Serial direction
  USBMIDI.setHandleNoteOn(onNoteOn);
  USBMIDI.setHandleNoteOff(onNoteOff);
  USBMIDI.setHandleControlChange(onControlChange);
  USBMIDI.setHandleProgramChange(onProgramChange);
  USBMIDI.setHandlePitchBend(onPitchBend);
  USBMIDI.setHandleAfterTouch(onAfterTouch);
  USBMIDI.setHandlePolyPressure(onPolyPressure);
  USBMIDI.setHandleRealTime(onRealTime);
}

// =================================================================================
// DIRECTION 1: USB -> SERIAL (Callbacks)
// =================================================================================

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  MIDI_SERIAL.write(0x90 | (channel & 0x0F));
  MIDI_SERIAL.write(note);
  MIDI_SERIAL.write(velocity);
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
  MIDI_SERIAL.write(0x80 | (channel & 0x0F));
  MIDI_SERIAL.write(note);
  MIDI_SERIAL.write(velocity);
}

void onControlChange(uint8_t channel, uint8_t control, uint8_t value) {
  MIDI_SERIAL.write(0xB0 | (channel & 0x0F));
  MIDI_SERIAL.write(control);
  MIDI_SERIAL.write(value);
}

void onProgramChange(uint8_t channel, uint8_t program) {
  MIDI_SERIAL.write(0xC0 | (channel & 0x0F));
  MIDI_SERIAL.write(program);
}

void onPitchBend(uint8_t channel, int value) {
  // Re-map -8192..8191 to 14-bit (0..16383)
  uint16_t mapped = (uint16_t)(value + 8192);
  uint8_t lsb = mapped & 0x7F;
  uint8_t msb = (mapped >> 7) & 0x7F;
  MIDI_SERIAL.write(0xE0 | (channel & 0x0F));
  MIDI_SERIAL.write(lsb);
  MIDI_SERIAL.write(msb);
}

void onAfterTouch(uint8_t channel, uint8_t pressure) {
  MIDI_SERIAL.write(0xD0 | (channel & 0x0F));
  MIDI_SERIAL.write(pressure);
}

void onPolyPressure(uint8_t channel, uint8_t note, uint8_t pressure) {
  MIDI_SERIAL.write(0xA0 | (channel & 0x0F));
  MIDI_SERIAL.write(note);
  MIDI_SERIAL.write(pressure);
}

void onRealTime(uint8_t realtimebyte) {
  MIDI_SERIAL.write(realtimebyte);
}

// =================================================================================
// DIRECTION 2: SERIAL -> USB (Parser Loop)
// =================================================================================

// Parser State
uint8_t runningStatus = 0;
uint8_t msgType = 0;
uint8_t msgChannel = 0;
uint8_t dataByte1 = 0;
uint8_t dataByte2 = 0;
int msgState = 0; // 0: Wait Status, 1: Wait Data1, 2: Wait Data2

void loop() {
  // 1. Poll USB for incoming packets (trigger callbacks above)
  USBMIDI.poll();
  
  // 2. Read Serial for outgoing packets (Parser)
  while (MIDI_SERIAL.available()) {
    uint8_t b = MIDI_SERIAL.read();
    
    // --- RealTime Messages (Priority) ---
    if (b >= 0xF8) { 
      USBMIDI.sendRealTime(b);
      continue; // RealTime doesn't affect running status or parser state
    }
    
    // --- Status Byte (New Message) ---
    if (b & 0x80) {
      runningStatus = b;
      msgType = b & 0xF0;
      msgChannel = b & 0x0F;
      msgState = 1; // Expect Data 1 next
      
      // Handle special system common messages if needed (Tune Request 0xF6, etc)
      // For simplified bridge, we focus on Channel Voice messages (0x80-0xE0)
      if (msgType < 0x80) msgState = 0; // Ignore invalid
      continue;
    }
    
    // --- Data Bytes ---
    if (msgState == 0 && runningStatus != 0) {
      // Running Status: Use previous status
      msgType = runningStatus & 0xF0;
      msgChannel = runningStatus & 0x0F;
      msgState = 1;
    }
    
    if (msgState == 1) {
      dataByte1 = b;
      
      // Check message length
      if (msgType == 0xC0 || msgType == 0xD0) {
        // 2-byte messages (Program Change, Channel Pressure) -> DONE
        if (msgType == 0xC0) USBMIDI.sendProgramChange(msgChannel, dataByte1);
        if (msgType == 0xD0) USBMIDI.sendAfterTouch(msgChannel, dataByte1);
        msgState = 0; // Reset for next message (Running Status stays active)
      } else {
        // 3-byte messages -> Wait for next byte
        msgState = 2;
      }
    } else if (msgState == 2) {
      dataByte2 = b;
      
      // 3-byte messages DONE
      switch (msgType) {
        case 0x80: USBMIDI.sendNoteOff(msgChannel, dataByte1, dataByte2); break;
        case 0x90: USBMIDI.sendNoteOn(msgChannel, dataByte1, dataByte2); break;
        case 0xA0: USBMIDI.sendPolyPressure(msgChannel, dataByte1, dataByte2); break;
        case 0xB0: USBMIDI.sendControlChange(msgChannel, dataByte1, dataByte2); break;
        case 0xE0: 
          {
             int val = (dataByte1 & 0x7F) | ((dataByte2 & 0x7F) << 7);
             val -= 8192;
             USBMIDI.sendPitchBend(msgChannel, val);
          }
          break;
      }
      msgState = 0; // Reset (Running Status stays active)
    }
  }
}
