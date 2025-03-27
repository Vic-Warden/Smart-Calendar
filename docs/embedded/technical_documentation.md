# Technical documentation

Your Wiring Diagram, Bill of Materials, ... everything about how you built your embedded device should be documented here.

![Menu](Embedded_device.png)

---

## ESP32 ( Inputs )

__Button :__

Connected to PIN D14

VCC : 3.3 V

Resistance of 10k Ohm

__Photo resistance :__

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

__Oled Display :__

Connected to PIN A5 **SCL**

Connected to PIN A4 **SDA**

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