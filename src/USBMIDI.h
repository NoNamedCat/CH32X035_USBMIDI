#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "internal/wch_usbmidi_internal.h"

// Callback function types
typedef void (*MidiCallbackNote)(uint8_t channel, uint8_t note, uint8_t velocity);
typedef void (*MidiCallbackCC)(uint8_t channel, uint8_t control, uint8_t value);
typedef void (*MidiCallbackPC)(uint8_t channel, uint8_t program);
typedef void (*MidiCallbackPB)(uint8_t channel, int value); // Value -8192 to 8191
typedef void (*MidiCallbackCP)(uint8_t channel, uint8_t pressure); // Channel Pressure
typedef void (*MidiCallbackPP)(uint8_t channel, uint8_t note, uint8_t pressure); // Poly Pressure
typedef void (*MidiCallbackRT)(uint8_t realtimebyte); // 0xF8 clock, 0xFA start, etc.

class USBMIDI_ {
public:
    void begin();
    
    // Low level packet send
    void sendPacket(uint8_t cin, uint8_t b1, uint8_t b2, uint8_t b3);
    
    // High Level Send
    void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
    void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity = 0);
    void sendControlChange(uint8_t channel, uint8_t control, uint8_t value);
    void sendProgramChange(uint8_t channel, uint8_t program);
    void sendPitchBend(uint8_t channel, int value);
    void sendPolyPressure(uint8_t channel, uint8_t note, uint8_t pressure);
    void sendAfterTouch(uint8_t channel, uint8_t pressure); // Channel Pressure
    void sendRealTime(uint8_t realtimebyte); // e.g. 0xF8

    // Callback Registration
    void setHandleNoteOn(MidiCallbackNote func);
    void setHandleNoteOff(MidiCallbackNote func);
    void setHandleControlChange(MidiCallbackCC func);
    void setHandleProgramChange(MidiCallbackPC func);
    void setHandlePitchBend(MidiCallbackPB func);
    void setHandleAfterTouch(MidiCallbackCP func);
    void setHandlePolyPressure(MidiCallbackPP func);
    void setHandleRealTime(MidiCallbackRT func);

    // Poll for incoming data
    void poll();

private:
    MidiCallbackNote cbNoteOn = nullptr;
    MidiCallbackNote cbNoteOff = nullptr;
    MidiCallbackCC cbControlChange = nullptr;
    MidiCallbackPC cbProgramChange = nullptr;
    MidiCallbackPB cbPitchBend = nullptr;
    MidiCallbackCP cbAfterTouch = nullptr;
    MidiCallbackPP cbPolyPressure = nullptr;
    MidiCallbackRT cbRealTime = nullptr;

    void dispatch(uint8_t cin, uint8_t b1, uint8_t b2, uint8_t b3);
};

extern USBMIDI_ USBMIDI;