# CS335 Final: Camp Half-Blood Sorting System

A two-board Arduino project that uses a color sensor to assign a Camp Half-Blood cabin, then displays a themed reveal on a TFT screen. Pressing the scanner button starts a ten-second color sampling period; the scanner averages the readings, classifies the color, and sends the cabin name to the display over I²C.

## Hardware and libraries

The repository contains firmware but no wiring schematic or exact board model. Use two Arduino-compatible boards with suitable I²C/SPI support, a TCS34725 color sensor, an HX8357D TFT display, a pushbutton, and a speaker/buzzer compatible with your board's output circuitry.

Install these libraries through the Arduino IDE Library Manager, including dependencies it requests:

- Adafruit TCS34725 for the scanner.
- Adafruit GFX Library and Adafruit HX8357 Library for the display.

`Wire` and `SPI` are provided by the selected board's Arduino core.

## Sketches and connections

| Sketch | Role | Pins / bus configuration in source |
| --- | --- | --- |
| [AryamanCode.ino](AryamanCode.ino) | Color scanner and I²C controller | Button: 2 (`INPUT_PULLUP`); speaker: 3; sensor on I²C |
| [TanushCode.ino](TanushCode.ino) | TFT display and I²C receiver | Address: `0x04`; TFT CS: 10, DC: 9, RST: 8; hardware SPI |

Connect the boards' SDA/SCL lines and common ground, and wire the sensor to the I²C bus. Wire the button between pin 2 and ground. Use the selected boards' actual SDA/SCL and hardware SPI pins, with power and logic levels appropriate to the boards and peripherals.

The display uses a 480×320 landscape layout. The message format is a single length byte followed by the uppercase cabin name.

## Build and upload

Each `.ino` is a separate firmware program with its own `setup()` and `loop()`. Do not compile both in the same sketch folder.

1. Create an `AryamanCode/` sketch folder containing only `AryamanCode.ino` and a separate `TanushCode/` folder containing only `TanushCode.ino`.
2. Open the scanner sketch in the Arduino IDE, select its board and port, then verify and upload it.
3. Open the display sketch, select the second board and port, then verify and upload it.
4. Open Serial Monitor at **9600 baud** to inspect scanner startup and transmitted/received cabin names.
5. Press the scanner button and hold the color target steady for ten seconds. Confirm that the display reveals the resulting cabin.

## Current limitations

The scanner can emit more cabin names than the display implements. The display supports Zeus, Poseidon, Athena, Hades, Apollo, Artemis, Ares, and Hermes; unrecognized names fall back to Hermes. Color thresholds are hardcoded and may need calibration for the sensor, lighting, and target material.

Hardware validation requires the actual boards and peripherals. No automated tests, board configuration, or circuit diagram are included.
