#include "USBMIDI.h"

USBMIDI_ USBMIDI;

void USBMIDI_::begin() {
    USB_init();
}

void USBMIDI_::sendPacket(uint8_t cin, uint8_t b1, uint8_t b2, uint8_t b3) {
    uint8_t packet[4];
    packet[0] = (cin & 0x0F); // Cable 0
    packet[1] = b1;
    packet[2] = b2;
    packet[3] = b3;
    USB_write(packet, 4);
}

// --- Send Functions ---

void USBMIDI_::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    sendPacket(0x09, 0x90 | (channel & 0x0F), note, velocity);
}

void USBMIDI_::sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
    sendPacket(0x08, 0x80 | (channel & 0x0F), note, velocity);
}

void USBMIDI_::sendControlChange(uint8_t channel, uint8_t control, uint8_t value) {
    sendPacket(0x0B, 0xB0 | (channel & 0x0F), control, value);
}

void USBMIDI_::sendProgramChange(uint8_t channel, uint8_t program) {
    sendPacket(0x0C, 0xC0 | (channel & 0x0F), program, 0);
}

void USBMIDI_::sendPitchBend(uint8_t channel, int value) {
    // Value -8192..8191 maps to 0..16383
    uint16_t mapped = (uint16_t)(value + 8192);
    uint8_t lsb = mapped & 0x7F;
    uint8_t msb = (mapped >> 7) & 0x7F;
    sendPacket(0x0E, 0xE0 | (channel & 0x0F), lsb, msb);
}

void USBMIDI_::sendPolyPressure(uint8_t channel, uint8_t note, uint8_t pressure) {
    sendPacket(0x0A, 0xA0 | (channel & 0x0F), note, pressure);
}

void USBMIDI_::sendAfterTouch(uint8_t channel, uint8_t pressure) {
    sendPacket(0x0D, 0xD0 | (channel & 0x0F), pressure, 0);
}

void USBMIDI_::sendRealTime(uint8_t realtimebyte) {
    // CIN 0x0F is for Single Byte messages (RealTime, TuneRequest)
    // 0xF8 = Clock, 0xFA = Start, 0xFB = Continue, 0xFC = Stop, 0xFE = ActiveSensing, 0xFF = Reset
    sendPacket(0x0F, realtimebyte, 0, 0);
}

// --- Callback Registration ---

void USBMIDI_::setHandleNoteOn(MidiCallbackNote func) { cbNoteOn = func; }
void USBMIDI_::setHandleNoteOff(MidiCallbackNote func) { cbNoteOff = func; }
void USBMIDI_::setHandleControlChange(MidiCallbackCC func) { cbControlChange = func; }
void USBMIDI_::setHandleProgramChange(MidiCallbackPC func) { cbProgramChange = func; }
void USBMIDI_::setHandlePitchBend(MidiCallbackPB func) { cbPitchBend = func; }
void USBMIDI_::setHandleAfterTouch(MidiCallbackCP func) { cbAfterTouch = func; }
void USBMIDI_::setHandlePolyPressure(MidiCallbackPP func) { cbPolyPressure = func; }
void USBMIDI_::setHandleRealTime(MidiCallbackRT func) { cbRealTime = func; }

// --- Reception ---

void USBMIDI_::poll() {
    uint8_t packet[4];
    while(USB_available() >= 4) {
        if(USB_read(packet, 4) == 4) {
            uint8_t cin = packet[0] & 0x0F;
            uint8_t b1 = packet[1];
            uint8_t b2 = packet[2];
            uint8_t b3 = packet[3];
            dispatch(cin, b1, b2, b3);
        }
    }
}

void USBMIDI_::dispatch(uint8_t cin, uint8_t b1, uint8_t b2, uint8_t b3) {
    uint8_t channel = b1 & 0x0F;
    
    switch(cin) {
        case 0x08: // Note Off
            if(cbNoteOff) cbNoteOff(channel, b2, b3);
            break;
            
        case 0x09: // Note On
            if(cbNoteOn && (b3 > 0)) cbNoteOn(channel, b2, b3);
            else if (cbNoteOff && (b3 == 0)) cbNoteOff(channel, b2, 0); // Vel 0 = Off
            break;
            
        case 0x0A: // Poly Key Pressure
            if(cbPolyPressure) cbPolyPressure(channel, b2, b3);
            break;
            
        case 0x0B: // Control Change
            if(cbControlChange) cbControlChange(channel, b2, b3);
            break;
            
        case 0x0C: // Program Change
            if(cbProgramChange) cbProgramChange(channel, b2);
            break;
            
        case 0x0D: // Channel Pressure (Aftertouch)
            if(cbAfterTouch) cbAfterTouch(channel, b2);
            break;
            
        case 0x0E: // Pitch Bend
            if(cbPitchBend) {
                // Reconstruct 14-bit value from LSB (b2) and MSB (b3)
                int val = (b2 & 0x7F) | ((b3 & 0x7F) << 7);
                val -= 8192; // Center at 0
                cbPitchBend(channel, val);
            }
            break;
            
        case 0x0F: // Single Byte (Real Time)
            if(cbRealTime) cbRealTime(b1);
            break;
            
        // 0x05 could be SysEx end OR standard 1-byte System Common (Tune Request 0xF6)
        case 0x05: 
            if(b1 >= 0xF8) { // If it's real time embedded here (rare but legal)
                if(cbRealTime) cbRealTime(b1);
            }
            break;
            
        default:
            // SysEx (0x04, 0x06, 0x07) and others ignored
            break;
    }
}
