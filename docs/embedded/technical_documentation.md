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
| ESP32-DEV-CP2102  | Espressif    | ESP32 development board with WiFi/Bluetooth | 1        | 9.95 €            | 9.95 €               | [Tinytronics](https://www.tinytronics.nl/en/development-boards/microcontroller-boards/with-wi-fi/esp32-wifi-and-bluetooth-board-cp2102)         | CP2102 USB-UART, dual-core MCU, 30 GPIO, 3.3V logic          |
| TACT-6x6x5        | Generic      | Tactile push button switch (momentary) | 1        | 0.15 €            | 0.15 €               | [Tinytronics](https://www.tinytronics.nl/en/switches/manual-switches/pcb-switches/tactile-push-button-switch-momentary-4pin-6*6*5mm)             | 6 x 6 x 5 mm, 4 pins, through-hole   |
| GL5528-LDR   | Generic      | Light dependent resistor (LDR)    | 1        | 0.40 €            | 0.40 €               | [Tinytronics](https://www.tinytronics.nl/en/sensors/optical/light-and-color/gl5528-ldr-light-sensitive-resistor)                         | GL5528, 10–20 kΩ in daylight, 5 mm, through-hole |
| PIR-HC-SR501  | Generic      | PIR motion sensor module         | 1        | 2.95 €            | 2.95 €               | [Tinytronics](https://www.tinytronics.nl/en/sensors/motion/ir-pyroelectric-infrared-pir-motion-sensing-detector-module)                 | HC-SR501, adjustable delay/sensitivity, 5V, digital output   |