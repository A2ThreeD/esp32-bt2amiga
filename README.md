# ESP32 Bluetooth to Amiga DB9 Adapter

This project connects Bluetooth input devices to an Amiga DB9 port using an ESP32.
It is built on Bluepad32 and supports gamepad/joystick and mouse-style usage.

## Build
1. Install ESP-IDF.
2. Clone this repository (with submodules).
3. Build and flash:
   - `idf.py build`
   - `idf.py -p <port> flash monitor`

## Runtime
- Uses Bluepad32 + btstack as the Bluetooth host stack.
- Reconnects to previously paired devices automatically.
- Maps controller and pointer input to Amiga DB9 output logic.

## Source Layout
- `main/main.cpp`: Bluepad32 platform glue and runtime behavior.
- `main/amiga-db9-mouse.cpp`: DB9 mouse signaling.
- `main/amiga-db9-joystick.cpp`: DB9 joystick signaling.

## Notes
- Legacy custom hostdev code has been removed in favor of the Bluepad32-based path.
