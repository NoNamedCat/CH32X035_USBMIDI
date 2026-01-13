![CH32X035 USB MIDI Library](img/bb7fb7ca-e147-4a9a-8a16-39f2ecf78176.png)

A full-featured USB MIDI device library for the WCH CH32X035 RISC-V microcontroller series.

## Features

- **Standard MIDI Messages:** Support for Note On/Off, Control Change, Program Change, Pitch Bend, Aftertouch (Channel & Poly), and Real-Time messages (Clock, Start, Stop, etc.).
- **Bi-directional:** Send and Receive MIDI messages with ease.
- **Callback System:** Register simple C++ functions to handle incoming MIDI events.
- **Efficient:** Built directly on top of the CH32X035 USBFS hardware for low latency.

## Getting Started

### Installation
1. Download this repository.
2. Place it in your Arduino `libraries` folder (e.g., `Documents/Arduino/libraries/CH32X035_USBMIDI`).
3. Restart the Arduino IDE.

### Basic Usage (Send Note)

```cpp
#include <USBMIDI.h>

void setup() {
  USBMIDI.begin();
}

void loop() {
  // Send Note On (Channel 1, Note 60, Velocity 127)
  USBMIDI.sendNoteOn(0, 60, 127);
  delay(500);
  
  // Send Note Off
  USBMIDI.sendNoteOff(0, 60, 0);
  delay(500);
  
  USBMIDI.poll(); // Keep USB alive
}
```

### Handling Input

```cpp
void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    // Do something
}

void setup() {
    USBMIDI.begin();
    USBMIDI.setHandleNoteOn(onNoteOn);
}

void loop() {
    USBMIDI.poll();
}
```

## Credits & Acknowledgements

This library was developed by **NoNamedCat**.

It is heavily based on the architecture of the **CH32X035_USBSerial** library. Huge thanks to the original author of the USB CDC implementation for providing a stable foundation for the low-level USBFS initialization and endpoint management.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
