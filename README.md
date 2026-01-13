![CH32X035 USB MIDI Library](img/bb7fb7ca-e147-4a9a-8a16-39f2ecf78176.png)

# CH32X035 USB MIDI Library

A comprehensive and efficient USB MIDI device library tailored for the **WCH CH32X035** RISC-V microcontroller series.

This library leverages the hardware USBFS peripheral to provide a low-latency, class-compliant MIDI interface. No drivers are needed on host operating systems (Windows, macOS, Linux, Android, iOS).

## Features

*   **Class Compliant:** Works natively without drivers on all major operating systems.
*   **Bi-directional Communication:** Send and receive MIDI messages seamlessly.
*   **Event-Driven Architecture:** Register efficient C++ callbacks to handle specific incoming messages (Note On/Off, Control Change, etc.), avoiding complex parsing in your main loop.
*   **Full Standard Support:** Supports the entire range of standard channel voice messages and real-time system messages.
*   **Hardware Optimized:** Built on top of the CH32X035 USBFS for minimal overhead.

## Supported MIDI Messages

This library supports sending and receiving the following MIDI specifications:

| Message Type | Send | Receive (Callback) | Description |
| :--- | :---: | :---: | :--- |
| **Note On** | ✅ | ✅ | Trigger a note |
| **Note Off** | ✅ | ✅ | Stop a note |
| **Control Change (CC)** | ✅ | ✅ | Knobs, faders, pedals |
| **Program Change (PC)** | ✅ | ✅ | Change instrument/preset |
| **Pitch Bend** | ✅ | ✅ | 14-bit pitch modification |
| **Aftertouch (Channel)** | ✅ | ✅ | Global pressure |
| **Poly Pressure** | ✅ | ✅ | Per-key pressure |
| **Real-Time Messages** | ✅ | ✅ | Clock, Start, Stop, Continue |

## Installation

1.  Download this repository as a ZIP file.
2.  Open the Arduino IDE.
3.  Go to **Sketch** -> **Include Library** -> **Add .ZIP Library...**
4.  Select the downloaded ZIP file.
5.  Restart the IDE.

## Usage Guide

### 1. Basic Setup

Include the library and initialize it in `setup()`. **Crucial:** You must call `USBMIDI.poll()` inside your `loop()` to process USB events.

```cpp
#include <USBMIDI.h>

void setup() {
    // Initialize USB MIDI stack
    USBMIDI.begin();
}

void loop() {
    // Must be called frequently to handle USB traffic
    USBMIDI.poll();
}
```

### 2. Sending MIDI Data

Send notes, control changes, and more using simple methods.

```cpp
void loop() {
    // Send Note On: Channel 1 (0-15), Note 60 (C4), Velocity 127
    USBMIDI.sendNoteOn(0, 60, 127);
    delay(500);

    // Send Note Off: Channel 1, Note 60
    USBMIDI.sendNoteOff(0, 60, 0);
    delay(500);

    // Send Control Change: Channel 1, Controller 1 (Mod Wheel), Value 100
    USBMIDI.sendControlChange(0, 1, 100);

    // Send Pitch Bend: Channel 1, Value 0 (Center) -> Range: -8192 to 8191
    USBMIDI.sendPitchBend(0, 0);
    
    // Handle USB communication
    USBMIDI.poll();
}
```

### 3. Receiving MIDI Data (Callbacks)

Instead of checking buffers manually, define functions that will be called automatically when a message arrives.

```cpp
// Define callback functions
void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    // Toggle an LED or play a sound
}

void handleControlChange(uint8_t channel, uint8_t controller, uint8_t value) {
    // Update internal parameters
}

void handleSystemRealTime(uint8_t realtimebyte) {
    if (realtimebyte == 0xF8) {
        // MIDI Clock tick
    }
}

void setup() {
    USBMIDI.begin();

    // Register callbacks
    USBMIDI.setHandleNoteOn(handleNoteOn);
    USBMIDI.setHandleControlChange(handleControlChange);
    USBMIDI.setHandleRealTime(handleSystemRealTime);
}

void loop() {
    USBMIDI.poll();
}
```

## Credits & Acknowledgements

This library was developed by **NoNamedCat**.

It is architecturally inspired by and built upon the **[CH32X035_USBSerial](https://github.com/jobitjoseph/CH32X035_USBSerial)** library.

Special thanks to **[jobitjoseph](https://github.com/jobitjoseph)** for his excellent work on the CH32X035 USB CDC implementation, which provided the essential foundation for the low-level USBFS initialization and endpoint management used in this project.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

