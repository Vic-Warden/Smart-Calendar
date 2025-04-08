# Technical documentation

Your Wiring Diagram, Bill of Materials, ... everything about how you built your embedded device should be documented here.

## **Wiring Diagram**

This Fritzing diagram shows the wiring of the ESP32-based Smart Calendar system. It includes the connection of several hardware components: an I2C LCD, a 7-segment display (TM1637), a DFPlayer Mini audio module, a servo motor, a PIR sensor, a brightness sensor (LDR), and a push-button. The aim is to visualize the project's physical architecture in order to facilitate assembly.

![Menu](Embedded_device.png)

---

## ESP32 ( Inputs )

__Button :__

Connected to PIN D14

VCC : 3.3 V

Resistance of 10k Ohm

__Photoresistor :__

Connected to PIN D34

VCC : 3.3 V

Resistance of 10k Ohm

__PIR Sensor:__

Connected to PIN D35

VCC : 5 V

---

## ESP32 ( Outputs )

__4 digits display :__

Connected to PIN D18 **CLK**

Connected to PIN D19 **DIO**

VCC : 3.3 V

__SV90 Servo :__

Connected to PIN D27

VCC : 5 V

__LCD Screen :__ 

Connected to PIN D22 **SCL**

Connected to PIN D21 **SDA**

VCC : 5 V

__DF Player Mini :__

Connected to PIN RX2 **TX**

Connected to PIN TX2 **RX**

VCC : 5 V

Resistance of 10k Ohm for **RX**

__Speaker :__

Connected to PIN SPK_1 **on DF Player mini**

Connected to PIN SPK_2 **on DF Player mini**

---

## **Bill of Materials**

This table lists all the components required to build the ESP32-based Smart Calendar System. Each item includes a description, the quantity used in the project, the estimated price (including VAT), and a link to an example supplier. These components provide the system's main functions: display, sensor reading, network communication and user interaction.

## 🧾 Bill of Materials

| Part #            | Manufacturer | Description                               | Quantity | Price (incl. VAT) | Subtotal (incl. VAT) | Example url                                                                                                                                       | Extra Info                                                   |
|-------------------|--------------|-------------------------------------------|----------|-------------------|----------------------|---------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------|
| ESP32-DEV  | Espressif    | ESP32 development board with WiFi/Bluetooth | 1        | 9.95 €            | 9.95 €               | [Tinytronics](https://www.tinytronics.nl/en/development-boards/microcontroller-boards/with-wi-fi/esp32-wifi-and-bluetooth-board-cp2102)         | CP2102 USB-UART, dual-core MCU, 30 GPIO, 3.3V logic          |
| TACT-6x6x5        | Generic      | Tactile push button switch (momentary) | 1        | 0.15 €            | 0.15 €               | [Tinytronics](https://www.tinytronics.nl/en/switches/manual-switches/pcb-switches/tactile-push-button-switch-momentary-4pin-6*6*5mm)             | 6 x 6 x 5 mm, 4 pins, through-hole   |
| GL5528-LDR   | Generic      | Light dependent resistor (LDR)    | 1        | 0.40 €            | 0.40 €               | [Tinytronics](https://www.tinytronics.nl/en/sensors/optical/light-and-color/gl5528-ldr-light-sensitive-resistor)                         | GL5528, 10–20 kΩ in daylight, 5 mm, through-hole |
| PIR-HC-SR501  | Generic      | PIR motion sensor module         | 1        | 2.95 €            | 2.95 €               | [Tinytronics](https://www.tinytronics.nl/en/sensors/motion/ir-pyroelectric-infrared-pir-motion-sensing-detector-module)                 | HC-SR501, adjustable delay/sensitivity, 5V, digital output   |
| 4SEG-COM-RED   | Generic      | 4-digit 7-segment display (red)      | 1        | 1.95 €            | 1.95 €               | [Tinytronics](https://www.tinytronics.nl/en/displays/segments/segmenten-display-4-characters-red)                                       | Common cathode, red color, 12 pins, 0.36", through-hole mount     |
| SG90      | TianKongRC   | SG90 Mini Servo    | 1        | 3.25 €            | 3.25 €               | [TinyTronics](https://www.tinytronics.nl/en/mechanics-and-actuators/motors/servomotors/sg90-mini-servo)                     | 4.8–6V, 1.8 kg·cm torque, 180° rotation, 9g weight |
| LCD-16x2-BL | Generic      | LCD Display 16x2 with blue backlight | 1        | 3.50 €            | 3.50 €               | [TinyTronics](https://www.tinytronics.nl/en/displays/lcd/lcd-display-16*2-characters-with-white-text-and-blue-backlight) | Parallel interface, white text on blue backlight, 5V logic |
| DFPlayerMini | DFRobot      | Mini MP3 Player Module    | 1        | 8.00 €            | 8.00 €               | [TinyTronics](https://www.tinytronics.nl/en/audio/audio-sources/dfrobot-dfplayer-mini-mp3-module)                           | Supports MP3/WAV/WMA, microSD up to 32GB, UART control, 3.2–5V supply, built-in 3W amplifier |
| WS-14595     | Waveshare    | Speaker Set - 8Ω 5W    | 1        | 6.75 €            | 6.75 €               | [TinyTronics](https://www.tinytronics.nl/en/audio/speakers/speakers/waveshare-speaker-set-8%CF%89-5w)         | 8Ω impedance, 5W rated power, 99x44.5x21mm size  |
| BB-170-WHITE  | Generic      | Breadboard 170 points (white) | 2        | 1.45 €            | 2.90 €               | [TinyTronics](https://www.tinytronics.nl/en/tools-and-mounting/prototyping-supplies/breadboards/breadboard-170-points-white)   | 170 tie-points, mini size, adhesive |
| BB-PWR-3.3V-5V     | Generic      | Breadboard Power Supply (3.3V/5V)  | 1        | 2.95 €            | 2.95 €               | [TinyTronics](https://www.tinytronics.nl/en/power/voltage-converters/voltage-regulators/breadboard-power-supply-5v-en-3.3v)              | Selectable 3.3V/5V output, micro USB & DC jack input, ~700mA max |
| DUP-MF-10CM      | Generic      | Dupont jumper wire M-F 10 cm (x10)      | 2 packs  | 1.25 € / pack     | 2.50 €               | [TinyTronics](https://www.tinytronics.nl/en/cables-and-connectors/cables-and-adapters/prototyping-wires/dupont-compatible-and-jumper/dupont-jumper-wire-male-female-10cm-10-wires) | Male to Female, 10 cm, 11 wires used out of 20     |
| DUP-MM-10CM      | Generic      | Dupont jumper wire M-M 10 cm (x10)      | 2 packs  | 1.25 € / pack     | 2.50 €               | [TinyTronics](https://www.tinytronics.nl/en/cables-and-connectors/cables-and-adapters/prototyping-wires/dupont-compatible-and-jumper/dupont-jumper-wire-male-male-10cm-10-wires) | Male to Male, 10 cm, 11 wires used out of 20      |
| RES-10K-5%   | Generic      | 10kΩ Resistor (±5%)          | 2        | 0.05 € / unit      | 0.10 €               | [TinyTronics](https://www.tinytronics.nl/en/components/resistors/resistors/10k%CF%89-resistor-(standard-pull-up-or-pull-down-resistor)) | Through-hole, 1/4W, sold per 10 pcs (0.50 €/10) |