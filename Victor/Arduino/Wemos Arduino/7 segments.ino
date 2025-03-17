#include <TM1637Display.h>

// Module connection pins (Digital Pins)
#define CLK 9
#define DIO 10

TM1637Display display(CLK, DIO);

void setup() {
  display.setBrightness(0x0f);
}

void loop() {
  display.showNumberDec(1234, false);
  delay(1000);
  display.showNumberDec(8765, false);
  delay(1000);
}