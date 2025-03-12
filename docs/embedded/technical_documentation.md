# Technical documentation

Your Wiring Diagram, Bill of Materials, ... everything about how you built your embedded device should be documented here.

![Menu](Embedded_device.png)

## Wemos ( Inputs )

__Button :__

Connected to PIN D0 

VCC : 3.3 V

Resistance of 10k Ohm

__Photo resistance :__

Connected to PIN A0

VCC : 3.3 V

Resistance of 10k Ohm

__PIR Sensor:__

Connected to PIN D2 

VCC : 5 V

---

## Arduino ( Outputs )

__LED :__

Connected to PIN 2

Resistance of 220 Ohm

__4 digits display :__

Connected to PIN 2 **CLK**

Connected to PIN 3 **DIO**

VCC : 5 V

__SV90 Servo :__

Connected to PIN 7

VCC : 5 V

__Oled Display :__

Connected to PIN A5 **SCL**

Connected to PIN A4 **SDA**

VCC : 5 V

__LCD Screen :__ **Not connected yet need to wait to receive it**

Connected to PIN ??

VCC : ??

Resistance of ?? Ohm

__DF Player Mini :__

Connected to PIN 10 **TX**

Connected to PIN 11 **RX**

VCC : 5 V

Resistance of 10k Ohm for **RX**

__Speaker :__

Connected to PIN SPK_1 **on DF Player mini**

Connected to PIN SPK_2 **on DF Player mini**


---

## Sends data between Wemos and arduino